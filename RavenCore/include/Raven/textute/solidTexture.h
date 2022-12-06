#ifndef _RAVEN_TEXTURE_SOLID_TEXTURE_H_
#define _RAVEN_TEXTURE_SOLID_TEXTURE_H_

#include<Raven/core/base.h>
#include<Raven/core/texture.h>
#include<Raven/utils/propertylist.h>

namespace Raven {
	template<class T>
	class CheckeredTexture :public Texture<T> {
	public:
		CheckeredTexture(
			const std::shared_ptr<Texture<T>>& oddTexture,
			const std::shared_ptr<Texture<T>>& evenTexture,
			const std::shared_ptr<TextureMapping2D>& mapping) :
			tex1(oddTexture), tex2(evenTexture), mapping(mapping) {}

		virtual	T evaluate(const SurfaceInteraction& its)const {
			auto [st, dstdx, dstdv] = mapping->map(its);
			if (((int)std::floor(st[0] * 10) + (int)std::floor(st[1] * 10)) % 2 == 0) {
				return tex1->evaluate(its);
			}
			else
				return tex2->evaluate(its);
		}

		//static std::optional<Ref<CheckeredTexture<T>>> construct(
		//	const PropertyList& params) {
		//	ObjectRef odd = params.getObjectRef();
		//	ObjectRef even = params.getObjectRef();
		//	ObjectRef mapping = params.getObjectRef();
		//	if (even.matchType == "nullptr" || odd.matchType == "nullptr" || even.matchType == "nullptr")
		//		std::cout << "Failed;checkered texture null ptr\.n";
		//	if (odd.matchType != "texture")
		//		std::cout << "Failed odd.\n";
		//	if (even.matchType != "texture")
		//		std::cout << "Failed even.\n";
		//	if (matchType != "mapping")
		//		std::cout << "Failed mapping.\n";
		//	return std::make_shared<CheckeredTexture<T>>(odd.getRef(), even.getRef(), mapping);
		//}
	private:
		std::shared_ptr<Texture<T>> tex1;
		std::shared_ptr<Texture<T>> tex2;
		std::shared_ptr<TextureMapping2D> mapping;
	};

	std::shared_ptr<CheckeredTexture<double>> makeCheckeredFloat(
		const std::shared_ptr<Texture<double>>& oddTexture,
		const std::shared_ptr<Texture<double>>& evenTexture,
		const std::shared_ptr<TextureMapping2D>& mapping,
		const PropertyList& param);

	std::shared_ptr<CheckeredTexture<Spectrum>> makeCheckeredSpectrum(
		const std::shared_ptr<Texture<Spectrum>>& oddTexture,
		const std::shared_ptr<Texture<Spectrum>>& evenTexture,
		const std::shared_ptr<TextureMapping2D>& mapping,
		const PropertyList& param);
}

#endif