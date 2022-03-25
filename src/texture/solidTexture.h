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

		T evaluate(const SurfaceInteraction& its)const;
		static std::shared_ptr<CheckeredTexture<T>> build(const std::shared_ptr<Texture<T>>& oddTexture,
			const std::shared_ptr<Texture<T>>& evenTexture);
	private:
		std::shared_ptr<Texture<T>> tex1;
		std::shared_ptr<Texture<T>> tex2;
	};
}

#endif