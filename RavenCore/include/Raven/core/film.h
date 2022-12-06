#ifndef _RAVEN_CORE_FILM_H_
#define _RAVEN_CORE_FILM_H_

#include<Raven/core/image.h>
#include<iostream>
#include<Raven/core/base.h>
#include<fstream>
#include<Raven/core/math.h>
#include<vector>
#include<Raven/core/spectrum.h>
#include<Raven/utils/propertylist.h>
#include<Raven/core/object.h>
#include<Raven/utils/factory.h>

namespace Raven {
//#define _RAVEN_FILM_REG_(regName,className,constructor)\
//	class className##Reg{\
//	private:\
//		className##Reg(){\
//			FilmFactory::regClass(#regName,constructor);\
//		}\
//		static className##Reg regHelper;\
//	};\

	class Film :public RavenObject {
	public:
		const int xRes, yRes;
		const double aspect_ratio;

		Film(int width, int height) :xRes(width), yRes(height), aspect_ratio(width / height), frameBuffer(width, height) {}
		Film(const Film& film)
			:xRes(film.xRes), yRes(film.yRes), aspect_ratio(film.aspect_ratio), frameBuffer(film.frameBuffer) {
		}
		void write()const;

		RGBSpectrum& operator()(int x, int y) {
			return frameBuffer(x, y);
		}
		const RGBSpectrum& operator()(int x, int y) const {
			return frameBuffer(x, y);
		}
		static Ref<Film> construct(const PropertyList& param) {
			int u = param.getInteger("width", 1080);
			int v = param.getInteger("height", 720);
			return std::make_shared<Film>(u, v);
		}
	private:
		Image<RGBSpectrum> frameBuffer;
	};

	_RAVEN_CLASS_REG_(film,Film,Film::construct)
}
#endif
