#include<Raven/material/glass.h>

namespace Raven {
	void Glass::computeScarttingFunctions(SurfaceInteraction& record)const {
		//�����µ�bsdf
		record.bsdf = std::make_shared<BSDF>(record, eta);

		//�������ֵ
		Spectrum kd = kdTex->evaluate(record);
		Spectrum kt = ktTex->evaluate(record);
		double uRoughValue = uRough->evaluate(record);
		double vRoughValue = vRough->evaluate(record);

		double etaI = record.eta;
		double etaT = this->eta;
		//���shading models
		bool isSpecular = uRoughValue == 0 && vRoughValue == 0;

		if (!kd.isBlack()) {
			//std::shared_ptr<Fresnel> fresnel = std::make_shared<FresnelDielectric>(1.0, eta);
			//record.bsdf->addBxDF(std::make_shared<SpecularReflection>(kd, fresnel));
		}
		if (!kt.isBlack()) {
			record.bsdf->addBxDF(std::make_shared<SpecularTransmission>(kt, 1.0, eta));
		}
	}
}