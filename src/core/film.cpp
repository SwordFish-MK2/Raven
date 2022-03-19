

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include<stb_image_write.h>
#include"film.h"
#include"../filter/gaussian.h"
#include"math.h"
#include<math.h>
namespace Raven {

	void Film::write()const {
		stbi_write_jpg("jpg_test.jpg", width, height, 3, data, 0);
	}

	void Film::writeTxt()const {
		std::ofstream outStream;
		outStream.open("jpg_data.txt", std::ios::out | std::ios::trunc);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int index = 3 * (i * height + j);
				outStream << "row: " << i << ",column : " << j << ", data: "
					<< (int)data[index] << " , " << (int)data[index + 1] << " , " << (int)data[index + 2] << std::endl;
			}
		}
	}

	//void Film::writeColor() {
	//	this->filter(30,1,1,1,1);
	//	unsigned char* colorData = (unsigned char*)malloc(sizeof(unsigned char) * 3 * height * width);
	//	for (int i = 0; i < height * width; i++) {
	//		for (int j = 0; j < 3; j++) {
	//			double c = std::sqrt(buffer[i].color[j]);//gamma correct
	//			int intC = static_cast<int>(255 * Clamp(c, 0.0, 0.999));
	//			colorData[i * 3 + j] = intC;
	//		}
	//	}
	//	stbi_write_jpg("filtered_jpg.jpg", width, height, 3, colorData, 0);
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
}