#ifndef _RAVEN_CAMERA_PROJECTIVE_CAMERA_H_
#define _RAVEN_CAMERA_PROJECTIVE_CAMERA_H_

#include"../core/camera.h"
#include"../core/base.h"

namespace Raven {

	class ProjectiveCamera :public Camera {
	public:
		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const = 0;
		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const = 0;
		ProjectiveCamera(const Transform& CTW, const Transform& STR, double lensRadius, double focalDistance) :
			Camera(CTW), ScreenToRaster(STR), RasterToScreen(STR.getInverseMatrix()), RasterToCamera(Identity()),
			CameraToScreen(Identity()),
			lensRadius(lensRadius), focalDistance(focalDistance) {
			//	RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
		}
	protected:
		Transform ScreenToRaster;//scale the scene from film space to raster space
		Transform RasterToScreen;
		Transform CameraToScreen;//project the scene into film space
		Transform RasterToCamera;//transform sample point from raster space to camera space

		//properties to generate depth of field
		double lensRadius;
		double focalDistance;
	};

	class PerspectiveCamera :public ProjectiveCamera {
	private:

		//datas to generate differential ray
		double dxCamera;
		double dyCamera;
	public:
		PerspectiveCamera(const Transform& CTW, const Transform& STR, double lensRadius, double focalDistance,
			double near, double far, double fov, double aspect_ratio) :
			ProjectiveCamera(CTW, STR, lensRadius, focalDistance) {
			CameraToScreen = Perspective(fov, aspect_ratio, near, far);
			RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
			//compute offset distance in camera space when sample point shift one pixel from the film
			dxCamera = RasterToCamera(Point3f(1.f, 0.f, 0.f)).x - RasterToCamera(Point3f(0.f, 0.f, 0.f)).x;
			dyCamera = RasterToCamera(Point3f(0.f, 1.f, 0.f)).y - RasterToCamera(Point3f(0.f, 0.f, 0.f)).y;
		}
		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const;
		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const;
	};

	class OrthographicCamera :public ProjectiveCamera {
	private:
		//datas to compute ray differential
		Vector3f dXOrigin;
		Vector3f dYOrigin;
	public:
		OrthographicCamera(const Transform& CTW, const Transform& STR, double lensRadius, double focalDistance,
			double top, double bottom, double left, double right, double near, double far) :
			ProjectiveCamera(CTW, STR, lensRadius, focalDistance) {
			CameraToScreen = Orthographic(top, bottom, left, right, near, far);
			RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
		}
		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const;
		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const;
	};

}

#endif