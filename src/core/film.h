#ifndef _RAVEN_CORE_FILM_H_
#define _RAVEN_CORE_FILM_H_

#include<iostream>
#include"base.h"
#include<fstream>
#include"math.h"
#include<vector>
#include"spectrum.h"

namespace Raven {
	//struct GeometryData {

	//	Normal3f n;
	//	Point3f p;
	//	Vector3f color;
	//	bool hit;
	//	GeometryData() :n(Normal3f(0.0)), color(Vector3f(0.0)), hit(false), p(0, 0, 0) {}
	//	GeometryData(const GeometryData& data) :n(data.n), color(data.color), p(data.p), hit(data.hit) {}
	//};

	class Film {
	public:
		const int height, width;
		const double aspect_ratio;

		Film(int w, int h) :height(h), width(w), aspect_ratio(w/h), index(0) {
			data = (unsigned char*)malloc(sizeof(unsigned char) * h * w * 3);
		}
		Film(const Film& f) :height(f.height), width(f.width), aspect_ratio(f.aspect_ratio), index(0)
		{
			data = (unsigned char*)malloc(sizeof(unsigned char) * height * width * 3);
			memcpy(data, f.data, sizeof(unsigned char) * height * width * 3);
		}
		~Film() {
			free(data);
		}
		void writeTxt()const;
		void write()const;
		//	void writeColor();
		void in(int value) {
			data[index++] = value;
		}
		void in(const Spectrum& color) {
			double invGamma = 1.0 / 2.2;
			for (int i = 0; i < 3; i++)
			{
				double c = GammaCorrect(color[i]);

				//double c = pow(color[i], invGamma);//gamma correct
				int intC = static_cast<int>(255 * Clamp(c, 0.0, 0.999));
				in(intC);
			}
		}

		//void setColor(const Vector3f& c, int x, int y) {
		//	int index = x + y * width;
		//	buffer[index].color = c;
		//}
		//void setGBuffer(GeometryData data, int x, int y) {
		//	int index = x + y * width;
		//	buffer[index] = data;
		//}
		//Vector3f getColor(int x, int y)const {
		//	Clamp(x, 0, width - 1);
		//	Clamp(y, 0, height - 1);
		//	index = y * width + x;
		//	return buffer[index].color;
		//}
		//Point3f getDepth(int x, int y)const {
		//	Clamp(x, 0, width - 1);
		//	Clamp(y, 0, height - 1);
		//	int index = y * width + x;
		//	return buffer[index].p;
		//}
		//Normal3f getNormal(int x, int y)const {
		//	Clamp(x, 0, width - 1);
		//	Clamp(y, 0, height - 1);
		//	int index = y * width + x;
		//	return buffer[index].n;
		//}
		//void filter(int filterSize, double sigmaX, double sigmaC, double sigmaN, double sigmaD);

	private:
		mutable int index;
		unsigned char* data;

	};
}
#endif
