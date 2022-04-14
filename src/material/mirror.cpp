#include"mirror.h"
#include"../shading_model/specular.h"

namespace Raven {

	void Mirror::computeScarttingFunctions(SurfaceInteraction& record)const {
		record.bsdf = std::make_shared <BSDF>(record, record.eta);


		std::shared_ptr<Fresnel> fresnel = std::make_shared<FresnelNoOp>();

		Spectrum rValue = rTex->evaluate(record);
		if (!rValue.isBlack()) {
			record.bsdf->addBxDF(std::make_shared<SpecularReflection>(rValue, fresnel));
		}
	}

}