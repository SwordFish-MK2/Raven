#ifndef _RAVEN_CORE_RAY_H_
#define _RAVEN_CORE_RAY_H_

#include<Raven/core/base.h>
#include<Raven/core/math.h>

namespace Raven {

	class Ray {
	public:

		Ray(bool hasDifferential = false) :time(0), hasDifferential(hasDifferential) {}
		Ray(const Point3f& o, const Vector3f& d, double t = 0) :origin(o), dir(d), time(t), hasDifferential(false) {}
		Ray(const Ray& r) :origin(r.origin), dir(r.dir), time(r.time), hasDifferential(hasDifferential) {}

		Point3f position(double t)const {//t here is a parameter discribe how long the ray travals through ray direction
			return Point3f(origin + dir * t);
		}

		Point3f origin;
		Vector3f dir;
		double time;//ray starting time
		bool hasDifferential;
		mutable double tMax = std::numeric_limits<double>::infinity();
	};
	class RayDifferential :public Ray {
	public:
		//rayDifferential part
		Point3f originX, originY;
		Vector3f directionX, directionY;

		RayDifferential(const Point3f& o, const Vector3f& d, double t = 0.f) :Ray(o, d, t) {
			hasDifferential = false;
		}
		RayDifferential() :Ray(false) {}
		RayDifferential(const Ray& ray) :Ray(ray) { hasDifferential = true; }
		void scaleRayDifferential(double s) {
			originX = origin + (originX - origin) * s;
			originY = origin + (originY - origin) * s;
			directionX = dir + (directionX - dir) * s;
			directionY = dir + (directionY - dir) * s;
		}
	};
}

#endif