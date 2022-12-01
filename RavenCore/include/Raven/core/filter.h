#ifndef _RAVEN_CORE_FILTER_H_
#define _RAVEN_CORE_FILTER_H_

#include<Raven/core/math.h>
#include<Raven/core/base.h>
#include<Raven/core/object.h>

namespace Raven {
	class Filter:RavenObject {
	public:
		//����Ҫ����ĵ�����˲��˵ľ���
		virtual double evaluate(const Point2f& p)const = 0;
		Filter(const Vector2f& r) :radius(r), invRadius(Vector2f(1 / r.x, 1 / r.y)) {}
	protected:
		Vector2f radius, invRadius;//radiusΪ�˲���x��y�����ϵİ뾶
	};

	class Filter1D {
	public:
		virtual double evaluate(double d)const = 0;
		Filter1D(double d) :radius(d), invRadius(1.0 / d) {}
	protected:
		double radius, invRadius;
	};
}

#endif
