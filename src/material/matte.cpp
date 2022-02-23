#include"matte.h"

namespace Raven {
	////determing bump effect and generate BSDF
	//void MatteMaterial::computeScarttingFunctions(surfaceIntersection& its)const {

	//	its.bsdf = std::make_shared <BSDF>(its, its.eta);
	//	//calculate values of textures 
	//	double sigValue = sigma->evaluate(its);
	//	Spectrum kdValue = kd->evaluate(its);
	//	if (sigValue == 0.f) {
	//		// if sigma = 0£¬generate lambertian reflection
	//		its.bsdf->addBxDF(std::make_shared<LambertainReflection>( kdValue));
	//	}
	//	else {
	//		//sigma not equal to 0,generate OrenNayar assumption model
	//		its.bsdf->addBxDF(std::make_shared<OrenNayar>(kdValue, sigValue));
	//	}
	//}

		//determing bump effect and generate BSDF
	void MatteMaterial::computeScarttingFunctions(SurfaceInteraction& its)const {

		its.bsdf = std::make_shared <BSDF>(its, its.eta);
		//calculate values of textures 
		double sigValue = sigma->evaluate(its);
		Vector3f kdValue = kd->evaluate(its);
		if (sigValue == 0.f) {
			// if sigma = 0£¬generate lambertian reflection
			its.bsdf->addBxDF(std::make_shared<LambertainReflection>(kdValue));
		}
		else {
			//sigma not equal to 0,generate OrenNayar assumption model
			its.bsdf->addBxDF(std::make_shared<OrenNayar>(kdValue, sigValue));
		}
	}
}