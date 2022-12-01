//#ifndef _RAVEN_CORE_MEDIA_H_
//#define _RAVEN_CORE_MEDIA_H_
//
//#include<Raven/core/base.h>
//#include<Raven/core/math.h>
//#include<Raven/core/spectrum.h>
//#include<Raven/core/object.h>
//
//namespace Raven {
//	class PhaseFunction {
//	public:
//		//计算Phase Function 的值
//		virtual double p(const Vector3f& wi, const Vector3f& wo)const = 0;
//	};
//
//	class HenyeyGreensteinPhaseFunction :public PhaseFunction {
//	public:
//		double p(const Vector3f& wi, const Vector3f& wo)const override;
//	private:
//		double g;
//	};
//
//	class Medium {
//	public:
//		//假设传入该函数的光线在传播过程中未被遮挡并且完全包围在当前介质中，计算光线的beam transmittance
//		virtual Spectrum Tr(const Ray& ray, Sampler& sampler)const = 0;
//	};
//
//	inline double PhaseHG(double cosTheta, double g) {
//		double inv4PI = 0.25 * M_PI;
//		double denom = 1 + g * g + 2 * g*(cosTheta);
//		return inv4PI * (1 - g * g) / denom * sqrt(denom);
//	}
//
//	
//}
//#endif