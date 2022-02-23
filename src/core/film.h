#ifndef _RAVEN_CORE_FILM_H_
#define _RAVEN_CORE_FILM_H_

#include<iostream>
#include"base.h"
#include<fstream>
#include"math.h"

namespace Raven {
	class Film {
	public:
		const int height, width;
		const double aspect_ratio;

		Film(int w, int h, double ratio) :height(h), width(w), aspect_ratio(ratio), index(0) {
			data = (unsigned char*)malloc(sizeof(unsigned char) * h * w * 3);
		}
		Film(const Film& f) :height(f.height), width(f.width), aspect_ratio(f.aspect_ratio), index(0) {
			data = (unsigned char*)malloc(sizeof(unsigned char) * height * width * 3);
			memcpy(data, f.data, sizeof(unsigned char) * height * width * 3);
		}

		~Film() {
			free(data);
		}

		void writeTxt()const;

		void write()const;

		void in(int value) {
			data[index++] = value;
		}

		void in(Vector3f color) {
			for (int i = 0; i < 3; i++)
			{
				//double c = GammaCorrect(color[i]);
				double c = sqrt(color[i]);//gamma correct
				int intC = static_cast<int>(255 * Clamp(c, 0.0, 0.999));
				in(intC);
			}
		}

	private:
		mutable int index;
		unsigned char* data;
	};
}

#endif