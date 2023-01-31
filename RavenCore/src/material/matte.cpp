#include<Raven/material/matte.h>

namespace Raven {
	//determing bump effect and generate BSDF
	void MatteMaterial::computeScarttingFunctions(
		SurfaceInteraction& its,
		bool allowMultipleLobes)const {

		if (bump != nullptr)
			Bump(this->bump, its);

		its.bsdf = std::make_shared <BSDF>(its);
		//calculate values of textures 
		double sigValue = sigma->evaluate(its);
		Spectrum kdValue = kd->evaluate(its);
		if (sigValue == 0.f) {
			// if sigma = 0，generate lambertian reflection
			its.bsdf->addBxDF(std::make_shared<LambertainReflection>(kdValue));
		}
		else {
			//sigma not equal to 0,generate OrenNayar assumption model
			its.bsdf->addBxDF(std::make_shared<OrenNayar>(kdValue, sigValue));
		}
	}

	MatteMaterialReg MatteMaterialReg::regHelper;

}