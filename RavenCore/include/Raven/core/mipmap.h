#ifndef _RAVEN_CORE_MIPMAP_H_
#define _RAVEN_CORE_MIPMAP_H_

#include<Raven/core/spectrum.h>
#include<Raven/core/base.h>
#include<Raven/core/math.h>
#include<Raven/core/image.h>
#include<Raven/utils/memory.h>

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
		Mipmap(const Image<T>& image, bool trilinear = true, ImageWrap wrap = ImClamp);
		T lookup(const Point2f& uv, double width=0.0)const;
		T lookup(const Point2f& st, const Vector2f& dstdx, const Vector2f& dstdy)const;

		int maxL()const {
			return maxLevel;
		}

		//返回第i层的图像
		Image<T>* getLevel(int i) {
			i = Clamp(i, 0, (int64_t)maxLevel - 1);
			return pyramid[i].get();
		}

		void log();

	private:
		const bool doTrilinear;
		const ImageWrap wrapMode;
		Point2i resolution;
		int maxLevel;
		std::vector<std::unique_ptr<Image<T>>> pyramid;

		T triangle(int level, const Point2f& st)const;
		const T& texel(int level, int s, int t)const;
		double lanczosSinc(double v, double tau = 2)const;
		std::vector<ResampleWeight> resampleWeights(int32_t oldRes, int32_t newRes);
	};
}

#endif