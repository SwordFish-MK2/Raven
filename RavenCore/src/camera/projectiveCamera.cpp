#include <Raven/camera/projectiveCamera.h>
#include <Raven/core/film.h>

#include <iostream>
#include <memory>
#include <optional>

namespace Raven {
ProjectiveCamera::ProjectiveCamera(const Transform&        CTW,
                                   const Transform&        CTS,
                                   const Bound2f&          screenWindow,
                                   double                  lensRadius,
                                   double                  focalDistance,
                                   std::unique_ptr<Film>   f,
                                   std::shared_ptr<Medium> medium)
    : Camera(CTW, std::move(f), medium),
      CameraToScreen(CTS),
      lensRadius(lensRadius),
      focalDistance(focalDistance) {
  // Raster
  ScreenToRaster =
      Scale(Vector3f(film->xRes, film->yRes, 1)) *
      Scale(Vector3f(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
                     1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1)) *
      Translate(-screenWindow.pMin.x, -screenWindow.pMax.y, 0.0);

  RasterToScreen = ScreenToRaster.inverse();
  RasterToCamera = CameraToScreen.inverse() * RasterToScreen;
}


OrthographicCamera::OrthographicCamera(const Transform&        CTW,
                                       const Bound2f&          screenWindow,
                                       double                  lensRadius,
                                       double                  focalDistance,
                                       double                  near,
                                       double                  far,
                                       std::unique_ptr<Film>   film,
                                       std::shared_ptr<Medium> medium)
    : ProjectiveCamera(CTW,
                       Orthographic(near, far),
                       screenWindow,
                       lensRadius,
                       focalDistance,
                       std::move(film),
                       medium) {}

std::optional<Ray> OrthographicCamera::generateRay(
    const CameraSample& sample) const {
  Point3f         pRaster(sample.filmSample[0], sample.filmSample[1], 0.0f);
  Point3f         pCamera = RasterToCamera(pRaster);
  RayDifferential sampleRay(pCamera, Vector3f(0, 0, 1), sample.time, medium);
  return CameraToWorld(sampleRay);
}

std::optional<RayDifferential> OrthographicCamera::generateRayDifferential(
    const CameraSample& sample) const {
  Point3f         pRaster(sample.filmSample[0], sample.filmSample[1], 0.0f);
  Point3f         pCamera = RasterToCamera(pRaster);
  RayDifferential sampleRay(pCamera, Vector3f{0, 0, 1}, sample.time, medium);
  return CameraToWorld(sampleRay);
}

PerspectiveCamera::PerspectiveCamera(const Transform&        CTW,
                                     double                  lensRadius,
                                     double                  focalDistance,
                                     double                  fov,
                                     std::unique_ptr<Film>   film,
                                     std::shared_ptr<Medium> medium)
    : ProjectiveCamera(CTW,
                       Perspective(fov, 1e-2f, 1000.f),
                       Bound2f(Point2f(-1, -1), Point2f(1, 1)),
                       lensRadius,
                       focalDistance,
                       std::move(film),
                       medium) {
  // compute offset distance in camera space when sample point shift one pixel
  // from the film
  dxCamera = RasterToCamera(Point3f(1.f, 0.f, 0.f)).x -
             RasterToCamera(Point3f(0.f, 0.f, 0.f)).x;
  dyCamera = RasterToCamera(Point3f(0.f, 1.f, 0.f)).y -
             RasterToCamera(Point3f(0.f, 0.f, 0.f)).y;
}

std::optional<Ray> PerspectiveCamera::generateRay(
    const CameraSample& sample) const {
  // starting the ray from the center of the lens
  Point3f  ori(0.0f, 0.0f, 0.0f);
  Point3f  pRaster(sample.filmSample[0], sample.filmSample[1],
                   0.0f);                      // point on film in raster space
  Point3f  pCamera = RasterToCamera(pRaster);  // point on film in camera space
  Vector3f dir(pCamera[0], pCamera[1], pCamera[2]);
  Ray      sampleRay(ori, dir.normalized(), sample.time, medium);
  // update the ray origin and properate direction from the lens sample
  if (lensRadius > 0.f) {  // depth of field would be computed only when the
                           // lensRadius is larger than 0.f
    double  t = focalDistance / dir.z;
    Point3f p = sampleRay.position(t);
    Point3f lensSamplePoint =
        lensRadius * Point3f(sample.lensSample.x, sample.lensSample.y, 0.f);
    Vector3f rayDirection = (p - lensSamplePoint).normalized();
    sampleRay.origin      = lensSamplePoint;
    sampleRay.dir         = rayDirection;
  }
  // transform ray to the world space
  return CameraToWorld(sampleRay);
}

std::optional<RayDifferential> PerspectiveCamera::generateRayDifferential(
    const CameraSample& sample) const {
  // compute main ray,same with GenerateRay
  Point3f         ori(0.0f, 0.f, 0.f);
  Point3f         pRaster(sample.filmSample[0], sample.filmSample[1], 0.0f);
  Point3f         pCamera = RasterToCamera(pRaster);
  Vector3f        dir(pCamera[0], pCamera[1], pCamera[2]);
  RayDifferential sampleRay(ori, dir.normalized(), sample.time, medium);
  sampleRay.hasDifferential = true;
  sampleRay.originX         = ori;
  sampleRay.originY         = ori;
  sampleRay.directionX      = Normalize(dir + Vector3f(dxCamera, 0.f, 0.f));
  sampleRay.directionY      = Normalize(dir + Vector3f(0.f, dyCamera, 0.f));

  if (lensRadius > 0.f) {
    double  t = focalDistance / dir.z;
    Point3f p = sampleRay.position(t);
    Point3f lensSamplePoint =
        lensRadius * Point3f(sample.lensSample[0], sample.lensSample[1], 0.f);
    Vector3f rayDirection = (p - lensSamplePoint).normalized();
    sampleRay.origin      = lensSamplePoint;
    sampleRay.dir         = rayDirection;

    // compute x differential ray
    Vector3f xdir     = Normalize(rayDirection + Vector3f(dxCamera, 0.f, 0.f));
    double   tx       = focalDistance / xdir.z;
    Point3f  px       = Point3f(tx * xdir);
    sampleRay.originX = lensSamplePoint;
    sampleRay.directionX = Normalize(px - lensSamplePoint);

    // compute y differential ray
    Vector3f ydir     = Normalize(rayDirection + Vector3f(0.f, dyCamera, 0.f));
    double   ty       = focalDistance / ydir.z;
    Point3f  py       = Point3f(ty * ydir);
    sampleRay.originY = lensSamplePoint;
    sampleRay.directionY = Normalize(py - lensSamplePoint);
  }
  return CameraToWorld(sampleRay);
}

// Ref<Camera> OrthographicCamera::construct(const PropertyList& param) {
//	ObjectRef CTWObj = param.getObjectRef(0);
//	ObjectRef STRObj = param.getObjectRef(1);
//	double lensRadius = param.getFloat("lensRadius", 0);
//	double focalDistance = param.getFloat("focalDis", 10);
//	double top = param.getFloat("top", 100);
//	double bottom = param.getFloat("bottom", -100);
//	double left = param.getFloat("left", -100);
//	double right = param.getFloat("right", 100);
//	double near = param.getFloat("near", 0.0001);
//	double far = param.getFloat("far", 1000);

//	Ref<Transform> CTW =
// std::dynamic_pointer_cast<Transform>(CTWObj.getRef()); 	Ref<Transform>
// STR = std::dynamic_pointer_cast<Transform>(STRObj.getRef());

//	return std::make_shared<OrthographicCamera>(*CTW, *STR, lensRadius,
// focalDistance, top, bottom, left, right, near, far);

//}

}  // namespace Raven