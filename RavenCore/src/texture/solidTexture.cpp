#include<Raven/textute/solidTexture.h>

namespace Raven {
	CheckeredTextureFloatReg CheckeredTextureFloatReg::regHelper;

	CheckeredTextureSpectraReg CheckeredTextureSpectraReg::regHelper;

	namespace TextureBuild {
		Ref<CheckeredTexture<double>> makeCheckeredFloat(const PropertyList& param) {
			ObjectRef oddT = param.getObjectRef(0);
			ObjectRef evenT = param.getObjectRef(1);
			ObjectRef mapping = param.getObjectRef(2);
			return std::make_shared<CheckeredTexture<double>>(oddT.getRef(), evenT.getRef(), mapping);
		}

		Ref<CheckeredTexture<Spectrum>> makeCheckeredSpectrum(const PropertyList& param) {
			ObjectRef oddT = param.getObjectRef(0);
			ObjectRef evenT = param.getObjectRef(1);
			ObjectRef mapping = param.getObjectRef(2);
			return std::make_shared<CheckeredTexture<Spectrum>>(oddT.getRef(), evenT.getRef(), mapping);
		}
	}
}

