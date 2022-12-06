#ifndef _RAVEN_CORE_CAMERA_H_
#define _RAVEN_CORE_CAMERA_H_

#include<Raven/core/base.h>
#include<Raven/core/math.h>
#include<Raven/core/transform.h>
#include<Raven/core/object.h>

namespace Raven {
	struct CameraSample {
		Point2f filmSample;
		double time;
		Point2f lensSample;
		CameraSample(const Point2f& film, double t, const Point2f& lens) :
			filmSample(film), time(t), lensSample(lens) {}
		CameraSample(double fu, double fv, double t, double lu, double lv) :
			filmSample(Point2f(fu, fv)), time(t), lensSample(Point2f(lu, lv)) {}
	};



	class Camera :public RavenObject {
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			Camera(const Transform& CTW) :CameraToWorld(CTW) {}

		virtual int GenerateRay(const CameraSample& sample, Ray& ray)const = 0;
		virtual int GenerateRayDifferential(const CameraSample& sample, RayDifferential& rayDifferential)const = 0;
	protected:
		Transform CameraToWorld;//place the camera in the scene including rotation and translation
	};

	class CameraFactory {
		using CameraConstructor = std::function<Ref<Camera>(const PropertyList&)>;
	public:
		static bool registed(const std::string& name);

		static void regClass(const std::string& name, const CameraConstructor& cons);

		static Ref<Camera> generateClass(const std::string& name, const PropertyList& param);

		static std::map<std::string, CameraConstructor>& getMap() {
			static std::map<std::string, CameraConstructor> map;
			return map;
		}
	};

#define _RAVEN_CAMERA_REG_(regName,className,constructor)\
	class className##Reg{\
	private:\
		className##Reg() {\
			CameraFactory::regClass(#regName,constructor);\
		}\
		static className##Reg className##Reg regHelper;\
	};

}
#endif