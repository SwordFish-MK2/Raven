#include<Raven/core/texture.h>

namespace Raven {
	namespace RConstTexture {

		Ref<Texture<double>> makeConstTextureFloat(const PropertyList& param) {
			double data = param.getFloat("data", 1.0);
			return std::make_shared<ConstTexture<double>>(data);
		}

		Ref<Texture<Spectrum>> makeConstTextureSpectrum(const PropertyList& param) {
			Spectrum data = param.getSpectra("reflectance", Spectrum(1.0));
			return std::make_shared<ConstTexture<Spectrum>>(data);
		}

		//	Ref<Texture<double>> makeScaleTextureFloat(const PropertyList& param) {
		//		ObjectRef t1 = param.getObjectRef(0);
		//		ObjectRef t2 = param.getObjectRef(1);

		//		return std::make_shared<ScaleTexture<double>>(t1.getRef(), t2.getRef());
		//	}

		//	Ref<Texture<Spectrum>> makeScaleTextureSpectrum(const PropertyList& param) {
		//		ObjectRef t1 = param.getObjectRef(0);
		//		ObjectRef t2 = param.getObjectRef(1);

		//		return std::make_shared<ScaleTexture<Spectrum>>(t1.getRef(), t2.getRef());
		//	}
		//}


	//	ConstTextureFloatReg ConstTextureFloatReg::regHelper;
	//
	//	ConstTextureSpectraReg ConstTextureSpectraReg::regHelper;
	//
	//	//ScaleTextureFloatReg ScaleTextureFloatReg::regHelper;
	//
	//	//ScaleTextureSpectraReg ScaleTextureSpectraReg::regHelper;
	}
}