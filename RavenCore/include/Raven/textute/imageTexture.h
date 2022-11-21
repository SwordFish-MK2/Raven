#ifndef _RAVEN_TEXTURE_IMAGE_TEXTURE_H_
#define _RAVEN_TEXTURE_IMAGE_TEXTURE_H_

#include<Raven/core/base.h>
#include<Raven/core/texture.h>
#include<map>
#include<string>
#include<Raven/core/mipmap.h>
#include<Raven/utils/propertylist.h>

namespace Raven {

	struct TextureInfo {
		std::string path;
		bool doTrilinear;//whether use triangle filer in lookup routine
		ImageWrap wrap;//Image wrap mode
		bool gamma;//whether texels need gamma correct
		TextureInfo() {}
		TextureInfo(const std::string& path, bool doTrilinear,
			ImageWrap wrap, bool gamma) :
			path(path), doTrilinear(doTrilinear),
			wrap(wrap), gamma(gamma) {}
		bool operator<(const TextureInfo& t)const {
			if (path != t.path)return path < t.path;
			if (doTrilinear != t.doTrilinear)return doTrilinear < t.doTrilinear;
			if (gamma != t.gamma)return !gamma;
			return wrap < t.wrap;
		}
	};

	/// <summary>
	/// 图像纹理
	/// 单通道纹理： TMemory = double, TReturn = double
	/// 三通道纹理： TMemory = RGBSpectrum, TReturn = Spectrum
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class TMemory, class TReturn>
	class ImageTexture final : public Texture<TMemory> {
	public:
		ImageTexture(
			const std::string path,
			const std::shared_ptr<TextureMapping2D>& mapping,
			bool doTrilinear,
			ImageWrap wrap,
			bool gamma,
			bool filter = false) :mapping(mapping), filter(filter) {
			map = getTexture(path, doTrilinear, wrap, gamma);
		}

		virtual	TReturn evaluate(const SurfaceInteraction& its)const;

		static std::shared_ptr<ImageTexture<TMemory, TReturn>> build(
			const std::string& path,
			const std::shared_ptr<TextureMapping2D>& mapping,
			bool dotrilinear,
			ImageWrap wrap,
			bool gamma) {
			return std::make_shared<ImageTexture<TMemory, TReturn>>(path, mapping, dotrilinear, wrap, gamma);
		}

	private:
		bool filter;
		Mipmap<TMemory>* map;
		std::shared_ptr<TextureMapping2D> mapping;

		static std::map<TextureInfo, std::unique_ptr<Mipmap<TMemory>>> textureMap;

		static Mipmap<TMemory>* getTexture(const std::string& path, bool doTrilinear,
			ImageWrap wrap, bool gamma);

		static void convertIn(const RGBSpectrum& from, double& to, bool gamma = true) {
			to = gamma ? InverseGammaCorrect(from.y()) : from.y();
		}

		static void convertIn(const RGBSpectrum& from, RGBSpectrum& to, bool gamma = true) {
			for (int i = 0; i < RGBSpectrum::sampleNumber; i++) {
				to.c[i] = gamma ? InverseGammaCorrect(from[i]) : from[i];
			}
		}

		static void convertOut(const RGBSpectrum& from, Spectrum& to) {
			double rgb[3];
			from.toRGB(rgb);
			to = Spectrum::fromRGB(rgb);
		}

		static void convertOut(double from, double& to) {
			to = from;
		}
	};

	std::shared_ptr<ImageTexture<double, double>> makeImageTextureFloat(
		const std::shared_ptr<TextureMapping2D>& mapping,
		const PropertyList& param);

	//std::shared_ptr<ImageTexture<RGBSpectrum, Spectrum>>makeImageTextureSpectrum(
	//	const std::shared_ptr<TextureMapping2D>& mapping,
	//	const PropertyList& param);
}

#endif