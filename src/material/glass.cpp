#include"glass.h"

namespace Raven {
	void Glass::computeScarttingFunctions(SurfaceInteraction& record)const {
		//生成新的bsdf
		record.bsdf = std::make_shared<BSDF>(record);

		//求纹理的值
		Spectrum kd = kdTex->evaluate(record);
		Spectrum kt = ktTex->evaluate(record);
		double uRoughValue = uRough->evaluate(record);
		double vRoughValue = vRough->evaluate(record);

		double etaI = record.eta;
		double etaT = this->eta;
		//添加shading models
		bool isSpecular = uRoughValue == 0 && vRoughValue == 0;

		if (!kd.isBlack()) {
			std::shared_ptr<Fresnel> fresnel = std::make_shared<FresnelDielectric>(1.0, eta);
			record.bsdf->addBxDF(std::make_shared<SpecularReflection>(kd, fresnel));
		}
		if (!kt.isBlack()) {
			record.bsdf->addBxDF(std::make_shared<SpecularTransmission>(kt, 1.0, eta));
		}
	}
}