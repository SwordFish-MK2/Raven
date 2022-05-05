#ifndef _RAVEN_CORE_IMAGE_H_
#define _RAVEN_CORE_IMAGE_H_

#include"base.h"
#include"math.h"
#include"spectrum.h"

namespace Raven {
	/// <summary>
/// 储存2D图像
/// 三通道图像(T为RGBSpectrum)：用于储存颜色贴图、渲染结果等
/// 单通道图像(T为double）：用于储存bump map等
/// </summary>
/// <typeparam name="T"></typeparam>
	template<class T>
	class Image {
	public:
		Image(int width, int height, const T* d = nullptr) :
			xRes(width), yRes(height) {
			size_t dataSize = xRes * yRes;
			data.resize(dataSize);
			if (d != nullptr)
				std::memcpy(&data[0], d, dataSize * sizeof(T));
		}

		Image(const Image& image) :xRes(image.xRes), yRes(image.yRes), data(image.data) {}

		int uSize() const { return xRes; }
		int vSize() const { return yRes; }

		void resize(int width, int height) {
			xRes = width;
			yRes = height;
			int newSize = width * height;
			data.resize(newSize);
		}

		void setPixel(const T& value, int x, int y) {
			int index = y * xRes + x;
			data[index] = value;
		}
		T getPixel(const T& value, int x, int y)const {
			int index = y * xRes + x;
			return data[index];
		}

		T& operator()(int x, int y) {
			int index = y * xRes + x;
			return data[index];
		}
		const T& operator()(int x, int y) const {
			int index = y * xRes + x;
			return data[index];
		}

		T& operator[](int i) {
			return data[i];
		}
		const T operator[](int i)const {
			return data[i];
		}

	private:
		int xRes, yRes;
		std::vector<T> data;
	};

	Spectrum toSpectrum(const unsigned char* data);

	Image<RGBSpectrum> ReadImage(const std::string& path);
}

#endif