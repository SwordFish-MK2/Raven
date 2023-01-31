#ifndef _RAVEN_FILTER_GAUSSIAN_H_
#define _RAVEN_FILTER_GAUSSIAN_H_

#include<Raven/core/filter.h>
#include<Raven/core/base.h>

namespace Raven {
	inline double Gaussian1D(double sigma, double d) {
		double invSig = 1.0 / (2 * sigma * sigma);
		return Max(0.0, exp(-invSig * d * d));
	}

	class GaussianFilter :public Filter {
	public:
		GaussianFilter(const Vector2f& r, double alpha) :Filter(r), alpha(alpha),
			expRX(exp(-alpha * r.x * r.x)), expRY(exp(-alpha * r.y * r.y)) {}
		virtual double evaluate(const Point2f& p)const {
			//2维高斯的值等于两个维度滤波的乘积
			return gaussian(p.x, expRX) * gaussian(p.y, expRY);
		}
	private:
		double expRX, expRY;//滤波在滤波边缘的值，减去该值确保滤波在边缘的值收敛到0
		double alpha;		//滤波的参数，控制波形的起伏大小
		double gaussian(double distance, double expv)const {
			//单个维度的高斯函数
			return Max(0.0, exp(-alpha * distance * distance) - expv);
		}
	};


}
#endif