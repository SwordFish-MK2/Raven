#include"projectiveCamera.h"

namespace Raven {

	int OrthographicCamera::GenerateRay(const CameraSample& sample, Ray& ray)const {
		Point3f ori(0.0f, 0.0f, 0.0f);
		Point3f pRaster(sample.filmSample[0], sample.filmSample[1], 0.0f);
		Point3f pCamera = RasterToCamera(pRaster);
		Vector3f dir(pCamera[0], pCamera[1], pCamera[2]);
		RayDifferential sampleRay(ori, dir.normalized());
		ray = CameraToWorld(sampleRay);
		return 1;
	}

	int OrthographicCamera::GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const {
		Point3f ori(0.0f, 0.0f, 0.0f);

		Point3f pRaster(sample.filmSample[0], sample.filmSample[1], 0.0f);
		Point3f pCamera = RasterToCamera(pRaster);
		Vector3f dir(pCamera[0], pCamera[1], pCamera[2]);
		RayDifferential sampleRay(ori, dir.normalized());
		rayDifferential = sampleRay;
		return 1;
	}

	int PerspectiveCamera::GenerateRay(const CameraSample& sample, Ray& ray)const {
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

	int PerspectiveCamera::GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const {
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

}