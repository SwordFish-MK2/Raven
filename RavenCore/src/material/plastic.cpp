#include<Raven/material/plastic.h>
#include<Raven/shading/lambertain.h>
#include<Raven/shading/microfacet.h>

namespace Raven {

	void Plastic::computeScarttingFunctions(SurfaceInteraction& hitRecord)const {

		std::shared_ptr<BSDF> bsdf = std::make_shared <BSDF>(hitRecord);

		//evaluate texture
		Spectrum kdValue = kd->evaluate(hitRecord);
		Spectrum ksValue = ks->evaluate(hitRecord);
		double roughValue = roughness->evaluate(hitRecord);

		//Ìí¼Óshading models

		//diffuse
		if (kdValue != Spectrum(0.0)) {
			std::shared_ptr<BxDF> lam = std::make_shared<LambertainReflection>(kdValue);
			bsdf->addBxDF(lam);
		}

		//glossy
		if (ksValue != Spectrum(0.0)) {
			std::shared_ptr<Fresnel> fresnel = std::make_shared<FresnelDielectric>(1.5f, 1.f);
			double alpha = GGX::RoughnessToAlpha(roughValue);
			std::shared_ptr<MicrofacetDistribution> distribute =
				std::make_shared<GGX>(alpha, alpha, false);

			std::shared_ptr<BxDF> spec =
				std::make_shared<MicrofacetReflection>(fresnel, distribute, ksValue);

			bsdf->addBxDF(spec);	
		}
		hitRecord.bsdf = bsdf;
	}

	std::shared_ptr<Plastic> Plastic::build(
		const std::shared_ptr<Texture<Spectrum>>& kd,
		const std::shared_ptr<Texture<Spectrum>>& ks,
		const std::shared_ptr<Texture<double>>& roughness,
		const std::shared_ptr<Texture<double>>& bump) {
		return std::make_shared<Plastic>(kd, ks, roughness, bump);
	}
}