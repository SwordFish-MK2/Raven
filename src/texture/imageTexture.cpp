#include"imageTexture.h"
#include"../utils/loader.h"
#include"../core/mipmap.h"
namespace Raven {
	template<class TMemory, class TReturn>
	std::map <TextureInfo, std::unique_ptr<Mipmap<TMemory>>> ImageTexture<TMemory, TReturn>::textureMap;

	template<class TMemory, class TReturn>
	TReturn ImageTexture<TMemory, TReturn>::evaluate(const SurfaceInteraction& its)const {
		auto [st, dstdx, dstdy] = mapping->map(its);

		//Image<TMemory>* texture = map->getLevel(0);
		//int s = std::floor(st.x * texture->uSize());
		//int t = std::floor(st.y * texture->vSize());
		//TMemory value = (*texture)(s,t);

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
		else {
			//读取图片并储存图片的RGB值
			Image<RGBSpectrum> imageData = ReadImage(path);

			//将图像的y轴反转（图像空间下图像的原点位于左上角，y轴向下，纹理坐标的原点位于左下角，t轴向上）
			for (int x = 0; x < imageData.uSize(); x++) {
				for (int y = 0; y < imageData.vSize(); y++) {
					int invY = imageData.vSize() - 1 - y;
					std::swap(imageData(x, y), imageData(x, invY));
				}
			}

			//TODO::!imageData

			//将图像从RGB格式转化为纹理指定的存储格式(RGBSpectrum或者Double)
			Image<TMemory> convertedImage(imageData.uSize(), imageData.vSize());
			for (int i = 0; i < imageData.uSize() * imageData.vSize(); i++) {
				convertIn(imageData[i], convertedImage[i], gamma);
			}

			//使用读取的图像生成Mipmap并存储在textureMap中
			Mipmap<TMemory>* mipmap = new Mipmap<TMemory>(convertedImage, doTrilinear, wrap);
			textureMap[info].reset(mipmap);
			return mipmap;
		}
	}

	std::shared_ptr<ImageTexture<double, double>> makeImageTextureFloat(
		const std::shared_ptr<TextureMapping2D>& mapping,
		const PropertyList& param) {
		std::string path = param.getString("filename");
		bool doTrilinear = param.getBoolean("trilinear");
		int wrap = param.getInteger("wrap");
		bool gamma = param.getBoolean("gamma");
		ImageWrap iwrap;
		switch (wrap) {
		case 0:
			iwrap = ImageWrap::ImClamp;
			break;
		case 1:
			iwrap = ImageWrap::ImRepeat;
			break;
		case 2:
			iwrap = ImageWrap::ImBlack;
			break;
		}
		return std::make_shared<ImageTexture<double, double>>(path, mapping, doTrilinear, iwrap, gamma);
	}

	std::shared_ptr<ImageTexture<RGBSpectrum, Spectrum>>makeImageTextureSpectrum(
		const std::shared_ptr<TextureMapping2D>& mapping,
		const PropertyList& param) {
		std::string path = param.getString("filename");
		bool doTrilinear = param.getBoolean("trilinear");
		int wrap = param.getInteger("wrap");
		ImageWrap iwrap;
		switch (wrap) {
		case 0:
			iwrap = ImageWrap::ImClamp;
			break;
		case 1:
			iwrap = ImageWrap::ImRepeat;
			break;
		case 2:
			iwrap = ImageWrap::ImBlack;
			break;
		}
		bool gamma = param.getBoolean("gamma");

		return std::make_shared<ImageTexture<RGBSpectrum, Spectrum>>(path, mapping, doTrilinear, iwrap, gamma);
	}

	//ImageTexture有两种形式
	template class ImageTexture<RGBSpectrum, Spectrum>;//以RGB格式存储，返回Raven正在使用的光谱格式
	template class ImageTexture<double, double>;//以浮点数的格式存储，返回浮点数格式
}


