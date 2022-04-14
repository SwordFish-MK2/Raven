//#ifndef _RAVEN_TEXTURE_IMAGE_TEXTURE_H_
//#define _RAVEN_TEXTURE_IMAGE_TEXTURE_H_
//
//#include"../core/base.h"
//#include"../core/texture.h"
//#include"../utils/mipmap.h"
//#include<map>
//#include<string>
//
//namespace Raven {
//
//	struct TextureInfo {
//		std::string path;
//		bool doTrilinear;//whether use triangle filer in lookup routine
//		ImageWrap wrap;//Image wrap mode
//		double scale;//parameter to scale texel value
//		bool gamma;//whether texels need gamma correct
//		TextureInfo() {}
//		TextureInfo(const std::string& path, bool doTrilinear,
//			ImageWrap wrap, double scale, bool gamma) :
//			path(path), doTrilinear(doTrilinear),
//			wrap(wrap), scale(scale), gamma(gamma) {}
//	};
//
//	template<class T>
//	class ImageTexture final : public Texture<T> {
//	public:
//		ImageTexture(
//			const std::string path,
//			const std::shared_ptr<TextureMapping2D>& mapping,
//			bool doTrilinear,
//			ImageWrap wrap,
//			double scale,
//			bool gamma) :mapping(mapping) {
//		//	map = GetTexture(path, doTrilinear, wrap, scale, gamma);
//	//		map = new Mipmap<T>(Point2f(1, 1), nullptr, true);
//		}
//
//		virtual	T evaluate(const SurfaceInteraction& its)const;
//
//		static std::shared_ptr<ImageTexture<T>> build(
//			const std::string& path,
//			const std::shared_ptr<TextureMapping2D>& mapping,
//			bool doTrilinear,
//			ImageWrap wrap,
//			double scale,
//			bool gamma) {
//			return std::make_shared<ImageTexture<T>>(path, mapping, doTrilinear, wrap, scale, gamma);
//		}
//	private:
//		Mipmap<T>* map;
//		std::shared_ptr<TextureMapping2D> mapping;
//
//		static std::map <TextureInfo, std::unique_ptr<Mipmap<T>>> textureMap;
//
//		static Mipmap<T>* GetTexture(const std::string& path, bool doTrilinear,
//			ImageWrap wrap, double scale, bool gamma);
//		static Spectrum scaleAndGamma(const Spectrum& original, double scale = 1.0, bool gamma = true);
//		static Spectrum InvScaleAndGamma(const Spectrum& original, bool gamma);
//
//
//	};
//
//
//}
//
//#endif