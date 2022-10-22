#include"solidTexture.h"

namespace Raven {
	std::shared_ptr<CheckeredTexture<double>> makeCheckeredFloat(
		const std::shared_ptr<Texture<double>>& oddTexture,
		const std::shared_ptr<Texture<double>>& evenTexture,
		const std::shared_ptr<TextureMapping2D>& mapping,
		const PropertyList& param) {
		return std::make_shared<CheckeredTexture<double>>(oddTexture, evenTexture, mapping);
	}

	std::shared_ptr<CheckeredTexture<Spectrum>> makeCheckeredSpectrum(
		const std::shared_ptr<Texture<Spectrum>>& oddTexture,
		const std::shared_ptr<Texture<Spectrum>>& evenTexture,
		const std::shared_ptr<TextureMapping2D>& mapping,
		const PropertyList& param) {
		return std::make_shared<CheckeredTexture<Spectrum>>(oddTexture, evenTexture, mapping);
	}
}

