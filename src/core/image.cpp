#include"image.h"
#include"spectrum.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include<stb_image/stb_image.h>

namespace Raven {
	Image<RGBSpectrum> ReadImage(const std::string& path) {
		const char* filename = (char*)path.c_str();
		int w, h, c;
		unsigned char* data = stbi_load(filename, &w, &h, &c, 0);

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

	Spectrum toSpectrum(const unsigned char* data) {
		double value[3] = { double(data[0]) / 256.0,double(data[1]) / 256.0,double(data[2]) / 256.0 };
		return RGBSpectrum::fromRGB(&value[0]);
	}
}