#include"texture.h"

namespace Raven {
	template<class T>
	static std::shared_ptr<ConstTexture<T>> ConstTexture<T>::build(T value) {
		return std::make_shared<ConstTexture<T>>(value);
	}
	
	template<class T>
	static std::shared_ptr<ScaleTexture<T>> ScaleTexture<T>::build(const std::shared_ptr<Texture<T>>& vT, const std::shared_ptr<Texture<T>>& sT) {
		return std::make_shared<ScaleTexture<T>>(vT, sT);
	}



}