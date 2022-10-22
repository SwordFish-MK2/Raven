#ifndef _RAVEN_CORE_FILM_H_
#define _RAVEN_CORE_FILM_H_

#include"image.h"
#include<iostream>
#include"base.h"
#include<fstream>
#include"math.h"
#include<vector>
#include"spectrum.h"
#include"../utils/propertylist.h"
namespace Raven {
	//struct GeometryData {

	//	Normal3f n;
	//	Point3f p;
	//	Vector3f color;
	//	bool hit;
	//	GeometryData() :n(Normal3f(0.0)), color(Vector3f(0.0)), hit(false), p(0, 0, 0) {}
	//	GeometryData(const GeometryData& data) :n(data.n), color(data.color), p(data.p), hit(data.hit) {}
	//};


	//TODO::add filter
	class Film {
	public:
		const int xRes, yRes;
		const double aspect_ratio;

		Film(int width, int height) :xRes(width), yRes(height), aspect_ratio(width / height), frameBuffer(width, height) {}
		Film(const Film& film)
			:xRes(film.xRes), yRes(film.yRes), aspect_ratio(film.aspect_ratio), frameBuffer(film.frameBuffer) {
		}
		void write()const;

		//int uSize() const { return xRes; }
		//int vSize() const { return yRes; }

		//void testMipmap();

		RGBSpectrum& operator()(int x, int y) {
			return frameBuffer(x, y);
		}
		const RGBSpectrum& operator()(int x, int y) const {
			return frameBuffer(x, y);
		}

	private:
		Image<RGBSpectrum> frameBuffer;
	};

	inline std::shared_ptr<Film> makeFilm(const PropertyList& param) {
		int u = param.getInteger("uSize");
		int v = param.getInteger("vSize");
		return std::make_shared<Film>(u, v);
	}

}
#endif
