#include<Raven/textute/solidTexture.h>

namespace Raven {
	CheckeredTextureFloatReg CheckeredTextureFloatReg::regHelper;

	CheckeredTextureSpectraReg CheckeredTextureSpectraReg::regHelper;

	namespace TextureBuild {
		Ref<CheckeredTexture<double>> makeCheckeredFloat(const PropertyList& param) {
			const auto& oddT = std::dynamic_pointer_cast<Texture<double>>(param.getObjectRef(0).getRef());
			const auto& evenT = std::dynamic_pointer_cast<Texture<double>>(param.getObjectRef(1).getRef());
			const auto& mapping = std::dynamic_pointer_cast<TextureMapping2D>(param.getObjectRef(2).getRef());
			return std::make_shared<CheckeredTexture<double>>(oddT, evenT, mapping);
		}

		Ref<CheckeredTexture<Spectrum>> makeCheckeredSpectrum(const PropertyList& param) {
			const auto& oddT = std::dynamic_pointer_cast<Texture<Spectrum>>(param.getObjectRef(0).getRef());
			const auto& evenT = std::dynamic_pointer_cast<Texture<Spectrum>>(param.getObjectRef(1).getRef());
			const auto& mapping = std::dynamic_pointer_cast<TextureMapping2D>(param.getObjectRef(2).getRef());
			return std::make_shared<CheckeredTexture<Spectrum>>(oddT, evenT, mapping);
		}
	}
}

