#ifndef _RAVEN_CORE_CAMERA_H_
#define _RAVEN_CORE_CAMERA_H_

#include<Raven/core/base.h>
#include<Raven/core/math.h>
#include<Raven/core/transform.h>
#include<Raven/core/object.h>

namespace Raven {
	struct CameraSample {

		CameraSample(const Point2f& film, double t, const Point2f& lens) :
			filmSample(film), time(t), lensSample(lens) {}

		CameraSample(double fu, double fv, double t, double lu, double lv) :
			filmSample(Point2f(fu, fv)), time(t), lensSample(Point2f(lu, lv)) {}

		Point2f filmSample;
		double time;
		Point2f lensSample;
	};



	class Camera :public RavenObject {
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			Camera(const Transform& CTW,
				const Ref<Film>& film,
				const Ref<Medium>& medium) :
			CameraToWorld(CTW),
			film(film),
			medium(medium) {}

		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const = 0;
		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const = 0;
	protected:
		Transform CameraToWorld;//place the camera in the scene, including rotation and translation

	public:
		Ref<Film> film;	//相机的幕布
		Ref<Medium> medium;//相机所处的介质，空指针对应无介质
	};
}
#endif