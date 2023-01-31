#include<Raven/material/mirror.h>
#include<Raven/shading/specular.h>

namespace Raven {

	void Mirror::computeScarttingFunctions(
		SurfaceInteraction& record,
		bool allowMultipleLobes)const
	{
		if (bumpTex != nullptr)Bump(bumpTex, record);

		record.bsdf = std::make_shared <BSDF>(record);

		std::shared_ptr<Fresnel> fresnel = std::make_shared<FresnelNoOp>();

		Spectrum rValue = rTex->evaluate(record);
		if (!rValue.isBlack()) {
			record.bsdf->addBxDF(std::make_shared<SpecularReflection>(rValue, fresnel));
		}
	}

	MirrorReg MirrorReg::regHelper;
}