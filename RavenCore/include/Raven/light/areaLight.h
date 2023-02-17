#ifndef _RAVEN_LIGHT_AREA_LIGHT_H_
#define _RAVEN_LIGHT_AREA_LIGHT_H_


#include<Raven/core/base.h>
#include<Raven/core/light.h>
#include<Raven/utils/propertylist.h>
#include<Raven/utils/factory.h>

namespace Raven {
	/// <summary>
	/// DiffuseAreaLight实现类，该光源上的任意一点向空间中的任意方向辐射同等大小的Radiance
	/// </summary>
	class DiffuseAreaLight final:public AreaLight {
	public:
		DiffuseAreaLight(
			const Ref<Transform>& LTW,
			const Ref<Transform>& WTL,
			int nSamples,
			const Ref<Shape>& shape,
			const Spectrum& I) :
			AreaLight(LTW, WTL, (int)LightFlag::AreaLight, nSamples, shape), emittedRadiance(I) {}

		virtual Spectrum Li(const Interaction& p, const Vector3f& wi)const override;

		virtual Spectrum sampleLi(const Interaction& inter, const Point2f& uv,
			LightSample* lightSample)const override;

		virtual Spectrum power()const override;

		virtual double pdf_Li(const Interaction& inter, const Vector3f& wi)const override;

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