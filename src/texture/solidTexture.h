#ifndef _RAVEN_TEXTURE_SOLID_TEXTURE_H_
#define _RAVEN_TEXTURE_SOLID_TEXTURE_H_

#include"../core/base.h"
#include"../core/texture.h"

namespace Raven {
	template<class T>
	class CheckeredTexture :public Texture<T> {
	public:
		CheckeredTexture(const std::shared_ptr<Texture<T>>& oddTexture, const std::shared_ptr<Texture<T>>& evenTexture) :
			tex1(oddTexture), tex2(evenTexture) {}

		virtual	T evaluate(const SurfaceInteraction& its)const {
			auto uv = its.uv;
			if (((int)std::floor(uv[0] * 10) + (int)std::floor(uv[1] * 10)) % 2 == 0) {
				return tex1->evaluate(its);
			}
			else
				return tex2->evaluate(its);
		}

		static std::shared_ptr<CheckeredTexture<T>> build(const std::shared_ptr<Texture<T>>& oddTexture,
			const std::shared_ptr<Texture<T>>& evenTexture) {
			return std::make_shared<CheckeredTexture<T>>(oddTexture, evenTexture);
		}
	private:
		std::shared_ptr<Texture<T>> tex1;
		std::shared_ptr<Texture<T>> tex2;
	};
}

#endif