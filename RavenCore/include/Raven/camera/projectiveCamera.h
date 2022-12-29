#ifndef _RAVEN_CAMERA_PROJECTIVE_CAMERA_H_
#define _RAVEN_CAMERA_PROJECTIVE_CAMERA_H_

#include<Raven/core/camera.h>
#include<Raven/core/base.h>
#include<Raven/utils/propertylist.h>
#include<Raven/utils/factory.h>

namespace Raven {

	class ProjectiveCamera :public Camera {
	public:
		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const = 0;

		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const = 0;

		ProjectiveCamera(const Transform& CTW, const Transform& CTS, const Bound2f& screenWindow, double lensRadius,
			double focalDistance, const Ref<Film>& film, const Ref<Medium>& medium = nullptr);

	protected:
		Transform CameraToScreen, RasterToCamera;
		Transform ScreenToRaster, RasterToScreen;

		//properties to generate depth of field
		double lensRadius;
		double focalDistance;
	};

	class PerspectiveCamera :public ProjectiveCamera {
	public:
		PerspectiveCamera(const Transform& CTW,
			double lensRadius, double focalDistance,double fov,
			const Ref<Film>& film, const Ref<Medium>& medium = nullptr);

		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const;

		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const;

		//	static Ref<Camera> construct(const PropertyList& param) {
			//	ObjectRef CTWObj = param.getObjectRef(0);
			//	ObjectRef STRObj = param.getObjectRef(1);
			//	double lensRadius = param.getFloat("lensRadius", 0);
			//	double focalDistance = param.getFloat("focalDis", 10);
			//	double near = param.getFloat("near", 0.001);
			//	double far = param.getFloat("far", 1000.0);
			//	double fov = param.getFloat("fov", 45.0);
			//	double aspectRatio = param.getFloat("aspectRatio", 1.5);
			//	Ref<Transform> CTW = std::dynamic_pointer_cast<Transform>(CTWObj.getRef());
			//	Ref<Transform> STR = std::dynamic_pointer_cast<Transform>(STRObj.getRef());
			//	return std::make_shared<PerspectiveCamera>(*CTW, *STR, lensRadius, focalDistance, near, far, fov, aspectRatio);
			//}

	private:

		//datas to generate differential ray
		double dxCamera;
		double dyCamera;
	};

	//_RAVEN_CLASS_REG_(perspective, PerspectiveCamera, PerspectiveCamera::construct)

	class OrthographicCamera :public ProjectiveCamera {

	public:
		OrthographicCamera(const Transform& CTW, const Bound2f& screenWindow, double lensRadius, double focalDistance,
			double near, double far, const Ref<Film>& film, const Ref<Medium>& medium = nullptr);

		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const;

		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const;

		//	static Ref<Camera> construct(const PropertyList& param);

	private:
		//datas to compute ray differential
		Vector3f dXOrigin;
		Vector3f dYOrigin;
	};

	//_RAVEN_CLASS_REG_(orthographic, OrthographicCamera, OrthographicCamera::construct)

}

#endif