#ifndef _RAVEN_CAMERA_PROJECTIVE_CAMERA_H_
#define _RAVEN_CAMERA_PROJECTIVE_CAMERA_H_

#include<Raven/core/camera.h>
#include<Raven/core/base.h>
#include<Raven/utils/propertylist.h>

namespace Raven {

	class ProjectiveCamera :public Camera {
	public:
		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const = 0;

		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const = 0;

		ProjectiveCamera(const Transform& CTW, const Transform& STR, double lensRadius, double focalDistance);

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
	public:
		PerspectiveCamera(const Transform& CTW, const Transform& STR, double lensRadius, double focalDistance,
			double near, double far, double fov, double aspect_ratio);

		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const;

		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const;

		static Ref<Camera> construct(const PropertyList& param);

	private:

		//datas to generate differential ray
		double dxCamera;
		double dyCamera;
	};

	_RAVEN_CLASS_REG_(perspective,PerspectiveCamera,PerspectiveCamera::construct)

	class OrthographicCamera :public ProjectiveCamera {

	public:
		OrthographicCamera(const Transform& CTW, const Transform& STR, double lensRadius, double focalDistance,
			double top, double bottom, double left, double right, double near, double far);

		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const;

		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const;

		static Ref<Camera> construct(const PropertyList& param);

	private:
		//datas to compute ray differential
		Vector3f dXOrigin;
		Vector3f dYOrigin;
	};

	_RAVEN_CLASS_REG_(orthographic,OrthographicCamera,OrthographicCamera::construct)

	//std::shared_ptr<PerspectiveCamera> makePerspectiveCamera(const Transform& CTW, const Transform& STR,
	//	const PropertyList& param);
	//std::shared_ptr<OrthographicCamera> makeOrthographicCamera(const Transform& CTW, const Transform& STR,
	//	const PropertyList& param);
}

#endif