#include"texture.h"

namespace Raven {

	
	template<class T>
	std::shared_ptr<ScaleTexture<T>> ScaleTexture<T>::build(const std::shared_ptr<Texture<T>>& vT,
		const std::shared_ptr<Texture<T>>& sT) {
		return std::make_shared<ScaleTexture<T>>(vT, sT);
	}



}