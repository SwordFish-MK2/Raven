#ifndef _RAVEN_LIGHT_AREA_LIGHT_H_
#define _RAVEN_LIGHT_AREA_LIGHT_H_


#include<Raven/core/base.h>
#include<Raven/core/light.h>
#include<Raven/utils/propertylist.h>

namespace Raven {
	/// <summary>
	/// DiffuseAreaLight实现类，该光源上的任意一点向空间中的任意方向辐射同等大小的Radiance
	/// </summary>
	class DiffuseAreaLight :public AreaLight {
	public:
		DiffuseAreaLight(
			const Transform* LTW,
			const Transform* WTL,
			int nSamples,
			const Shape* shape,
			const Spectrum& I) :
			AreaLight(LTW, WTL, (int)LightFlag::AreaLight, nSamples, shape), emittedRadiance(I) {}

		virtual Spectrum Li(const SurfaceInteraction& p, const Vector3f& wi)const;

		virtual Spectrum sampleLi(const SurfaceInteraction& inter, const Point2f& uv,
			LightSample* lightSample)const;

		virtual Spectrum power()const;

		virtual double pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const;
	private:
		const Spectrum emittedRadiance;
	};

	inline std::shared_ptr<DiffuseAreaLight> makeDiffuseAreaLight(
		const std::shared_ptr<Transform>& LTW,
		const std::shared_ptr<Transform>& WTL,
		const std::shared_ptr<Shape>& shape,
		const PropertyList& pList) {
		int nSamples = pList.getInteger("nsamples",2);
		Spectrum emit = pList.getSpectra("emit",Spectrum(1.0));
		return std::make_shared<DiffuseAreaLight>(LTW.get(), WTL.get(), nSamples, shape.get(), emit);
	}
}


#endif 