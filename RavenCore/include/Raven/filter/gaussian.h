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
			//2ά��˹��ֵ��������ά���˲��ĳ˻�
			return gaussian(p.x, expRX) * gaussian(p.y, expRY);
		}
	private:
		double expRX, expRY;//�˲����˲���Ե��ֵ����ȥ��ֵȷ���˲��ڱ�Ե��ֵ������0
		double alpha;		//�˲��Ĳ��������Ʋ��ε������С
		double gaussian(double distance, double expv)const {
			//����ά�ȵĸ�˹����
			return Max(0.0, exp(-alpha * distance * distance) - expv);
		}
	};


}
#endif