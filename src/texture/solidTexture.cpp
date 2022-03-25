#include"solidTexture.h"

namespace Raven {
	template<class T>
	T CheckeredTexture<T>::evaluate(const SurfaceInteraction& its)const {
		auto uv = its.uv;
		if (((int)std::floor(uv[0] * 10) + (int)std::floor(uv[1] * 10)) % 2 == 0) {
			return tex1->evaluate(its);
		}
		else
			return tex2->evaluate(its);
	}

	template<class T>
	std::shared_ptr<CheckeredTexture<T>> CheckeredTexture<T>::build(const std::shared_ptr<Texture<T>>& tOdd,
		const std::shared_ptr<Texture<T>>& tEven) {
		return std::make_shared<CheckeredTexture<T>>(tOdd, tEven);
	}
}

