#include<Raven/camera/projectiveCamera.h>

namespace Raven {
	ProjectiveCamera::ProjectiveCamera(
		const Transform& CTW,
		const Transform& STR,
		double lensRadius,
		double focalDistance) :
		Camera(CTW), ScreenToRaster(STR), RasterToScreen(STR.getInverseMatrix()), RasterToCamera(Identity()),
		CameraToScreen(Identity()),
		lensRadius(lensRadius), focalDistance(focalDistance) {
		//	RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
	}

	OrthographicCamera::OrthographicCamera(
		const Transform& CTW,
		const Transform& STR,
		double lensRadius,
		double focalDistance,
		double top,
		double bottom,
		double left,
		double right,
		double near,
		double far) :
		ProjectiveCamera(CTW, STR, lensRadius, focalDistance) {
		CameraToScreen = Orthographic(top, bottom, left, right, near, far);
		RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
	}

	int OrthographicCamera::GenerateRay(
		const CameraSample& sample,
		Ray& ray)const
	{
		Point3f ori(0.0f, 0.0f, 0.0f);
		Point3f pRaster(sample.filmSample[0], sample.filmSample[1], 0.0f);
		Point3f pCamera = RasterToCamera(pRaster);
		Vector3f dir(pCamera[0], pCamera[1], pCamera[2]);
		RayDifferential sampleRay(ori, dir.normalized());
		ray = CameraToWorld(sampleRay);
		return 1;
	}

	int OrthographicCamera::GenerateRayDifferential(
		const CameraSample& sample,
		RayDifferential& rayDifferential)const
	{
		Point3f ori(0.0f, 0.0f, 0.0f);

		Point3f pRaster(sample.filmSample[0], sample.filmSample[1], 0.0f);
		Point3f pCamera = RasterToCamera(pRaster);
		Vector3f dir(pCamera[0], pCamera[1], pCamera[2]);
		RayDifferential sampleRay(ori, dir.normalized());
		rayDifferential = sampleRay;
		return 1;
	}


	PerspectiveCamera::PerspectiveCamera(
		const Transform& CTW,
		const Transform& STR,
		double lensRadius,
		double focalDistance,
		double near,
		double far,
		double fov,
		double aspect_ratio) :
		ProjectiveCamera(CTW, STR, lensRadius, focalDistance)
	{
		CameraToScreen = Perspective(fov, aspect_ratio, near, far);
		RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;

		//compute offset distance in camera space when sample point shift one pixel from the film
		dxCamera = RasterToCamera(Point3f(1.f, 0.f, 0.f)).x - RasterToCamera(Point3f(0.f, 0.f, 0.f)).x;
		dyCamera = RasterToCamera(Point3f(0.f, 1.f, 0.f)).y - RasterToCamera(Point3f(0.f, 0.f, 0.f)).y;
	}

	int PerspectiveCamera::GenerateRay(
		const CameraSample& sample,
		Ray& ray)const
	{
		//starting the ray from the center of the lens
		Point3f ori(0.0f, 0.0f, 0.0f);
		Point3f pRaster(sample.filmSample[0], sample.filmSample[1], 0.0f);
		Point3f pCamera = RasterToCamera(pRaster);
		Vector3f dir(pCamera[0], pCamera[1], pCamera[2]);
		Ray sampleRay(ori, dir.normalized());
		//update the ray origin and properate direction from the lens sample
		if (lensRadius > 0.f) {//depth of field would be computed only when the lensRadius is larger than 0.f
			double t = focalDistance / dir.z;
			Point3f p = sampleRay.position(t);
			Point3f lensSamplePoint = lensRadius * Point3f(sample.lensSample.x, sample.lensSample.y, 0.f);
			Vector3f rayDirection = (p - lensSamplePoint).normalized();
			sampleRay.origin = lensSamplePoint;
			sampleRay.dir = rayDirection;
		}
		//transform ray to the world space
		ray = CameraToWorld(sampleRay);
		return 1;
	}

	int PerspectiveCamera::GenerateRayDifferential(
		const CameraSample& sample,
		RayDifferential& rayDifferential)const
	{
		//compute main ray,same with GenerateRay
		Point3f ori(0.0f, 0.f, 0.f);
		Point3f pRaster(sample.filmSample[0], sample.filmSample[1], 0.0f);
		Point3f pCamera = RasterToCamera(pRaster);
		Vector3f dir(pCamera[0], pCamera[1], pCamera[2]);
		RayDifferential sampleRay(ori, dir.normalized());
		sampleRay.hasDifferential = true;
		sampleRay.originX = ori;
		sampleRay.originY = ori;
		sampleRay.directionX = Normalize(dir + Vector3f(dxCamera, 0.f, 0.f));
		sampleRay.directionY = Normalize(dir + Vector3f(0.f, dyCamera, 0.f));

		if (lensRadius > 0.f) {
			double t = focalDistance / dir.z;
			Point3f p = sampleRay.position(t);
			Point3f lensSamplePoint = lensRadius * Point3f(sample.lensSample[0], sample.lensSample[1], 0.f);
			Vector3f rayDirection = (p - lensSamplePoint).normalized();
			sampleRay.origin = lensSamplePoint;
			sampleRay.dir = rayDirection;

			//compute x differential ray
			Vector3f xdir = Normalize(rayDirection + Vector3f(dxCamera, 0.f, 0.f));
			double tx = focalDistance / xdir.z;
			Point3f px = Point3f(tx * xdir);
			sampleRay.originX = lensSamplePoint;
			sampleRay.directionX = Normalize(px - lensSamplePoint);

			//compute y differential ray
			Vector3f ydir = Normalize(rayDirection + Vector3f(0.f, dyCamera, 0.f));
			double ty = focalDistance / ydir.z;
			Point3f py = Point3f(ty * ydir);
			sampleRay.originY = lensSamplePoint;
			sampleRay.directionY = Normalize(py - lensSamplePoint);
		}
		rayDifferential = CameraToWorld(sampleRay);
		return 1;
	}

	Ref<Camera> PerspectiveCamera::construct(const PropertyList& param) {
		ObjectRef CTWObj = param.getObjectRef(0);
		ObjectRef STRObj = param.getObjectRef(1);
		double lensRadius = param.getFloat("lensRadius", 0);
		double focalDistance = param.getFloat("focalDis", 10);
		double near = param.getFloat("near", 0.001);
		double far = param.getFloat("far", 1000.0);
		double fov = param.getFloat("fov", 45.0);
		double aspectRatio = param.getFloat("aspectRatio", 1.5);
		Ref<Transform> CTW = std::dynamic_pointer_cast<Transform>(CTWObj.getRef());
		Ref<Transform> STR = std::dynamic_pointer_cast<Transform>(STRObj.getRef());
		return std::make_shared<PerspectiveCamera>(*CTW, *STR, lensRadius, focalDistance, near, far, fov, aspectRatio);
	}

	Ref<Camera> OrthographicCamera::construct(const PropertyList& param) {
		ObjectRef CTWObj = param.getObjectRef(0);
		ObjectRef STRObj = param.getObjectRef(1);
		double lensRadius = param.getFloat("lensRadius", 0);
		double focalDistance = param.getFloat("focalDis", 10);
		double top = param.getFloat("top", 100);
		double bottom = param.getFloat("bottom", -100);
		double left = param.getFloat("left", -100);
		double right = param.getFloat("right", 100);
		double near = param.getFloat("near", 0.0001);
		double far = param.getFloat("far", 1000);

		Ref<Transform> CTW = std::dynamic_pointer_cast<Transform>(CTWObj.getRef());
		Ref<Transform> STR = std::dynamic_pointer_cast<Transform>(STRObj.getRef());

		return std::make_shared<Camera>(*CTW, *STR, lensRadius, focalDistance, top, bottom, left, right, near, far);

	}

}