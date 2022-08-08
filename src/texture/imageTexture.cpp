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

	//��ȡһ��������ȡ��Mipmap
	template<class TMemory, class TReturn>
	Mipmap<TMemory>* ImageTexture<TMemory, TReturn>::getTexture(
		const std::string& path,
		bool doTrilinear,
		ImageWrap wrap,
		bool gamma) {

		//���ϵͳ�Ѽ��ظ�ͼ�񣬷��ظ�ͼ���Mipmap
		TextureInfo info(path, doTrilinear, wrap, gamma);
		if (textureMap.find(info) != textureMap.end())
			return textureMap[info].get();

		//���ϵͳ��δ���ظ�ͼ�񣬶�ȡͼƬ������Mipmap
		else {
			//��ȡͼƬ������ͼƬ��RGBֵ
			Image<RGBSpectrum> imageData = ReadImage(path);

			//��ͼ���y�ᷴת��ͼ��ռ���ͼ���ԭ��λ�����Ͻǣ�y�����£����������ԭ��λ�����½ǣ�t�����ϣ�
			for (int x = 0; x < imageData.uSize(); x++) {
				for (int y = 0; y < imageData.vSize(); y++) {
					int invY = imageData.vSize() - 1 - y;
					std::swap(imageData(x, y), imageData(x, invY));
				}
			}

			//TODO::!imageData

			//��ͼ���RGB��ʽת��Ϊ����ָ���Ĵ洢��ʽ(RGBSpectrum����Double)
			Image<TMemory> convertedImage(imageData.uSize(), imageData.vSize());
			for (int i = 0; i < imageData.uSize() * imageData.vSize(); i++) {
				convertIn(imageData[i], convertedImage[i], gamma);
			}

			//ʹ�ö�ȡ��ͼ������Mipmap���洢��textureMap��
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

	//ImageTexture��������ʽ
	template class ImageTexture<RGBSpectrum, Spectrum>;//��RGB��ʽ�洢������Raven����ʹ�õĹ��׸�ʽ
	template class ImageTexture<double, double>;//�Ը������ĸ�ʽ�洢�����ظ�������ʽ
}


