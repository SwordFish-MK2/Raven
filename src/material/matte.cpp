#include"matte.h"

namespace Raven {
	//determing bump effect and generate BSDF
	void MatteMaterial::computeScarttingFunctions(SurfaceInteraction& its)const {

		its.bsdf = std::make_shared <BSDF>(its);
		//calculate values of textures 
		double sigValue = sigma->evaluate(its);
		Spectrum kdValue = kd->evaluate(its);
		if (sigValue == 0.f) {
			// if sigma = 0£¬generate lambertian reflection
			its.bsdf->addBxDF(std::make_shared<LambertainReflection>(kdValue));
		}
		else {
			//sigma not equal to 0,generate OrenNayar assumption model
			its.bsdf->addBxDF(std::make_shared<OrenNayar>(kdValue, sigValue));
		}
	}

	std::shared_ptr<MatteMaterial> MatteMaterial::buildConst(double sigma, const Spectrum& kd) {
		std::shared_ptr<Texture<double>> sigmaT = std::make_shared<ConstTexture<double>>(sigma);
		std::shared_ptr<Texture<Spectrum>> kdT = std::make_shared<ConstTexture<Spectrum>>(kd);

		return std::make_shared<MatteMaterial>(sigmaT, kdT);
	}

}