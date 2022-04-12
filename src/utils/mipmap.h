#ifndef _RAVEN_UTILS_MIPMAP_H_
#define _RAVEN_UTILS_MIPMAP_H_

#include"../core/spectrum.h"
#include"../core/base.h"

namespace Raven {
	enum ImageWrap {
		ImRepeat, ImBlack, ImClamp
	};

	struct ResampleWeight {
		int32_t firstTexel;
		double weight[4];
	};

	template<class T>
	class Mipmap {
	public:
		Mipmap(const Point2f& resolution, const T* imageData, bool trilinear = true, ImageWrap wrap = ImClamp);
		T lookup(const Point2f& uv, double width)const;
	private:
		const bool doTrilinear;
		const ImageWrap wrapMode;
		const Point2f resolution;
		int maxLevel;
		std::vector<std::unique_ptr<BlockedArray<T>>> pyramid;
		T triangle(int level, const Point2f& st)const;
		const T& texel(int level, int s, int t)const;
		double lanczosSinc(double v, double tau = 2)const;
		std::vector<ResampleWeight> resampleWeights(int32_t oldRes, int32_t newRes);
	};
}

#endif