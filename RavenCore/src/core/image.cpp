#include<Raven/core/image.h>
#include<Raven/core/spectrum.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif // !STB_IMAGE_WRITE_IMPLEMENTATION

#include<stb_image.h>

#include<stb_image_write.h>

namespace Raven {
	Image<RGBSpectrum> ReadImage(const std::string& path) {
		const char* filename = (char*)path.c_str();
		int w, h, c;
		unsigned char* data = stbi_load(filename, &w, &h, &c, 0);
		if (!data) {
			RGBSpectrum defaultV(1.0);
			return Image<RGBSpectrum>(1, 1, &defaultV);
		}
		std::unique_ptr<Spectrum[]> convertedData = std::make_unique<Spectrum[]>(w * h);
		if (c == 3) {
			for (int i = 0; i < w * h; i++)
				convertedData[i] = toSpectrum(&data[i * 3]);
		}
		else if (c == 4) {
			for (int i = 0; i < w * h; i++) {
				convertedData[i] = toSpectrum(&data[i * 4]);
			}
		}

		Image<RGBSpectrum> image(w, h, &(convertedData.get())[0]);
		return image;
	}

	
	void WriteImage(const Image<RGBSpectrum>& image, const std::string& filename) {
		unsigned char* colorData = (unsigned char*)malloc(sizeof(unsigned char) * 3 * image.uSize() * image.vSize());

		//遍历每个像素
		for (int y = 0; y < image.vSize(); y++) {
			for (int x = 0; x < image.uSize(); x++) {
				const RGBSpectrum& pixelValue = image(x, y);
				for (int i = 0; i < 3; i++) {
					int offset = 3 * (y * image.uSize() + x) + i;
					double value = GammaCorrect(pixelValue[i]);
					colorData[offset] = static_cast<int>(255 * Clamp(value, 0.0, 0.999));
				}
			}
		}
		stbi_write_jpg(filename.c_str(), image.uSize(), image.vSize(), 3, colorData, 0);
		free(colorData);
	}

	void WriteImage(const Image<double>& image, const std::string& filename) {
		unsigned char* colorData = (unsigned char*)malloc(sizeof(unsigned char) * 3 * image.uSize() * image.vSize());

		//遍历每个像素
		for (int y = 0; y < image.uSize(); y++) {
			for (int x = 0; x < image.vSize(); x++) {
				const RGBSpectrum& pixelValue = RGBSpectrum(image(x, y));
				for (int i = 0; i < 3; i++) {
					int offset = 3 * (y * image.uSize() + x) + i;
					double value = GammaCorrect(pixelValue[i]);
					colorData[offset] = static_cast<int>(255 * Clamp(value, 0.0, 0.999));
				}
			}
		}
		stbi_write_jpg(filename.c_str(), image.uSize(), image.vSize(), 3, colorData, 0);
		free(colorData);
	}

	Spectrum toSpectrum(const unsigned char* data) {
		double value[3] = { double(data[0]) / 256.0,double(data[1]) / 256.0,double(data[2]) / 256.0 };
		return RGBSpectrum::fromRGB(&value[0]);
	}

}