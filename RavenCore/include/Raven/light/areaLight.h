#ifndef _RAVEN_LIGHT_AREA_LIGHT_H_
#define _RAVEN_LIGHT_AREA_LIGHT_H_


#include<Raven/core/base.h>
#include<Raven/core/light.h>
#include<Raven/utils/propertylist.h>
#include<Raven/utils/factory.h>

namespace Raven {
	/// <summary>
	/// DiffuseAreaLightʵ���࣬�ù�Դ�ϵ�����һ����ռ��е����ⷽ�����ͬ�ȴ�С��Radiance
	/// </summary>
	class DiffuseAreaLight :public AreaLight {
	public:
		DiffuseAreaLight(
			const Ref<Transform>& LTW,
			const Ref<Transform>& WTL,
			int nSamples,
			const Ref<Shape>& shape,
			const Spectrum& I) :
			AreaLight(LTW, WTL, (int)LightFlag::AreaLight, nSamples, shape), emittedRadiance(I) {}

		virtual Spectrum Li(const SurfaceInteraction& p, const Vector3f& wi)const;

		virtual Spectrum sampleLi(const SurfaceInteraction& inter, const Point2f& uv,
			LightSample* lightSample)const;

		virtual Spectrum power()const;

		virtual double pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const;

		static Ref<Light> construct(const PropertyList& param) {
			const int n = param.getInteger("nSamples", 1);
			const Spectrum I = param.getSpectra("intansity", Spectrum(1.0));
			const ObjectRef s = param.getObjectRef(0);
			const Ref<Shape> shape = std::dynamic_pointer_cast<Shape>(s.getRef());
			return std::make_shared<DiffuseAreaLight>(nullptr, nullptr, n, shape, I);
		}

	private:
		const Spectrum emittedRadiance;
	};

	_RAVEN_CLASS_REG_(area,DiffuseAreaLight,DiffuseAreaLight::construct)

}


#endif 