#include<Raven/core/texture.h>

namespace Raven {
	template<class T>
	std::shared_ptr<ScaleTexture<T>> ScaleTexture<T>::build(const std::shared_ptr<Texture<T>>& vT,
		const std::shared_ptr<Texture<T>>& sT) {
		return std::make_shared<ScaleTexture<T>>(vT, sT);
	}

	std::shared_ptr<ConstTexture<double>> makeConstTextureFloat(const PropertyList& param) {
		double data = param.getFloat("data",1.0);
		return std::make_shared<ConstTexture<double>>(data);
	}

	std::shared_ptr<ConstTexture<Spectrum>> makeConstTextureSpectrum(const PropertyList& param) {
		Spectrum data = param.getSpectra("data",Spectrum(1.0));
		return std::make_shared<ConstTexture<Spectrum>>(data);
	}


}