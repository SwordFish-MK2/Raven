#include<Raven/material/glass.h>
#include<Raven/shading/microfacet.h>
namespace Raven {

	void Glass::computeScarttingFunctions(
		SurfaceInteraction& record,
		bool allowMultipleLobes)const {
		if (bumpTex != nullptr)Bump(bumpTex, record);

		//从纹理中取值
		double eta = etaTex->evaluate(record);
		Spectrum kd = kdTex->evaluate(record);
		Spectrum kt = ktTex->evaluate(record);
		double uRoughValue = uRough->evaluate(record);
		double vRoughValue = vRough->evaluate(record);

		//生成新的bsdf
		record.bsdf = std::make_shared<BSDF>(record, eta);

		if (kd.isBlack() && kt.isBlack())return;

		//添加shading models
		bool isSpecular = uRoughValue == 0 && vRoughValue == 0;

		std::shared_ptr<Fresnel> fresnel = std::make_shared<FresnelDielectric>(1.0, eta);
		if (isSpecular && allowMultipleLobes) {
			record.bsdf->addBxDF(std::make_shared<FresnelSpecular>(kd, kt, 1.0, eta));
		}
		else {
			//remap roughness to alpha
			if (remapRoughness) {
				uRoughValue = GGX::RoughnessToAlpha(uRoughValue);
				vRoughValue = GGX::RoughnessToAlpha(vRoughValue);
			}

			Ref<MicrofacetDistribution> dis = std::make_shared<GGX>(uRoughValue, vRoughValue, false);
			if (!kd.isBlack()) {
				Ref<Fresnel> fresnel = std::make_shared<FresnelDielectric>(1.0, eta);
				if (isSpecular)
					record.bsdf->addBxDF(std::make_shared<SpecularReflection>(kd, fresnel));
				else
					record.bsdf->addBxDF(std::make_shared<MicrofacetReflection>(fresnel, dis, kd));
			}
			else if (!kt.isBlack()) {
				if (isSpecular)
					record.bsdf->addBxDF(std::make_shared<SpecularTransmission>(kt, 1.0, eta));
				//else
				//	record.bsdf->addBxDF(std::make_shared<MicrofacetTransmi)
			}
		}
	}

	// GlassReg GlassReg::regHelper;
}