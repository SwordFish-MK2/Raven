#include"film.h"

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include<stb_image_write.h>

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
}