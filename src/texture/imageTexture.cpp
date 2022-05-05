#include"imageTexture.h"
#include"../utils/loader.h"

namespace Raven {
	template<class TMemory, class TReturn>
	std::map <TextureInfo, std::unique_ptr<Mipmap<TMemory>>> ImageTexture<TMemory, TReturn>::textureMap;

	template<class TMemory, class TReturn>
	TReturn ImageTexture<TMemory, TReturn>::evaluate(const SurfaceInteraction& its)const {
		auto [st, dstdx, dstdy] = mapping->map(its);
		TMemory value = map->lookup(st, dstdx, dstdy);
		TReturn result;
		convertOut(value, result);
		return result;
	}

	//读取一张纹理，获取其Mipmap
	template<class TMemory, class TReturn>
	Mipmap<TMemory>* ImageTexture<TMemory, TReturn>::getTexture(
		const std::string& path,
		bool doTrilinear,
		ImageWrap wrap,
		bool gamma) {

		//如果系统已加载该图像，返回该图像的Mipmap
		TextureInfo info(path, doTrilinear, wrap, gamma);
		if (textureMap.find(info) != textureMap.end())
			return textureMap[info].get();

		//如果系统尚未加载该图像，读取图片并生成Mipmap

		//读取图片
		Image<RGBSpectrum> imageData = ReadImage(path);

		//将图像的y轴反转（图像空间下图像的原点位于左上角，y轴向下，纹理坐标的原点位于左下角，t轴向上）
		for (int x = 0; x < imageData.uSize(); x++) {
			for (int y = 0; y < imageData.vSize(); y++) {
				int invY = imageData.vSize() - 1 - y;
				std::swap(imageData(x, y), imageData(x, invY));
			}
		}

		//TODO::!imageData
		Image<TMemory> convertedImage(imageData.uSize(), imageData.vSize());
		for (int i = 0; i < imageData.uSize() * imageData.vSize(); i++) {
			convertIn(imageData[i], convertedImage[i], gamma);
		}
		Mipmap<TMemory>* mipmap = new Mipmap<TMemory>(convertedImage, doTrilinear, wrap);

		textureMap[info].reset(mipmap);
		return mipmap;
	}

	template class ImageTexture<RGBSpectrum, Spectrum>;
	template class ImageTexture<double, double>;
}


