#include<Raven/core/film.h>
#include<Raven/filter/gaussian.h>
#include<Raven/core/math.h>
#include<math.h>
#include<Raven/core/mipmap.h>

namespace Raven {

	FilmReg FilmReg::regHelper;

	void Film::write()const {
		WriteImage(frameBuffer, "result.jpg");
	}


	//void Film::testMipmap() {
	//	Mipmap<RGBSpectrum> mipmap(frameBuffer, true, ImClamp);
	//	std::cout << std::endl << "Mipmap max level = " << mipmap.maxL() << std::endl;
	//	Image<RGBSpectrum>* ilevelImage = mipmap.getLevel(0);

	//	int uSize = ilevelImage->uSize();
	//	int vSize = ilevelImage->vSize();
	//	unsigned char* colorData = (unsigned char*)malloc(sizeof(unsigned char) * 3 * uSize * vSize);

	//	//遍历每个像素
	//	for (int y = 0; y < vSize; y++) {
	//		for (int x = 0; x < uSize; x++) {
	//			const RGBSpectrum& pixelValue = (*ilevelImage)(x, y);
	//			for (int i = 0; i < 3; i++) {
	//				int offset = 3 * (y * uSize + x) + i;
	//				double value = GammaCorrect(pixelValue[i]);
	//				colorData[offset] = static_cast<int>(255 * Clamp(value, 0.0, 0.999));
	//			}
	//		}
	//	}

	//	stbi_write_jpg("mipmapResult.jpg", uSize, vSize, 3, colorData, 0);
	//	free(colorData);
	//}

	//void Film::filter(int filterRadius, double sigmaX, double sigmaC, double sigmaN, double sigmaD) {
	//	for (int y = 0; y < height; y++) {
	//		for (int x = 0; x < width; x++) {
	//			//对每一个像素，执行高斯filter
	//			double totalWeight = 0;
	//			Vector3f totalColor(0.0);
	//			GeometryData gbuffer = buffer[x + y * width];
	//			//双边联合滤波
	//			for (int fy = -filterRadius; fy <= filterRadius; fy++) {
	//				for (int fx = -filterRadius; fx <= filterRadius; fx++) {

	//					int px = Clamp(x + fx, 0, width - 1);
	//					int py = Clamp(y + fy, 0, height - 1);
	//					GeometryData t = buffer[px + py * width];

	//					double wx = Gaussian1D(sigmaX, pow(px - x, 2));
	//					double wy = Gaussian1D(sigmaX, pow(py - y, 2));

	//					double dn = acos(Dot(gbuffer.n, t.n));
	//					double wn = Gaussian1D(sigmaN, pow(dn, 2));

	//					Vector3f dc = gbuffer.color - t.color;
	//					double wc = Gaussian1D(sigmaC, dc.lengthSquared());

	//					Vector3f nv = (gbuffer.p - t.p).normalized();
	//					double dPlane = Dot(nv, gbuffer.n);
	//					double wp = Gaussian1D(sigmaD, pow(dPlane, 2));
	//					
	//					double w = wx * wy * wn * wc * wp;
	//					totalWeight += w;
	//					totalColor += w * t.color;
	//					std::cout << "wx:" << wx << " ,wy: " << wy << " ,wn:" << wn << " ,wc:" << wc << " ,wp" << wp << std::endl;
	//				}
	//			}
	//			if (totalWeight != 0) {
	//				Vector3f result = totalColor / totalWeight;
	//				setColor(result, x, y);
	//			}
	//			//else {
	//			//	Vector3f result(0.0);
	//			//	setColor(result, x, y);
	//			//}

	//		}
	//	}
	//}


	void FilmFactory::regClass(const std::string& className, const FilmConstructor& constructor) {
		FilmConstructor cons = constructor;
		std::pair<std::string, FilmConstructor> my_pair = std::make_pair(className, cons);
		auto& my_class = FilmFactory::getMap();
		my_class.insert(my_pair);
	}

	bool FilmFactory::registed(const std::string& className) {
		auto& my_class = FilmFactory::getMap();
		auto classIt = my_class.find(className);
		return classIt != my_class.end();
	}

	Ref<Film> FilmFactory::generateClass(const std::string& className, const PropertyList& params) {
		auto& my_class = FilmFactory::getMap();
		auto classIt = my_class.find(className);
		Ref<Film> my_object = classIt->second(params);
		return my_object;
	}
}
