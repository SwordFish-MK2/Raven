#ifndef _RAVEN_SHAPE_SPHERE_H_
#define _RAVEN_SHAPE_SPHERE_H_


#include"../core/base.h"
#include"../core/shape.h"

namespace Raven {
	class Sphere :public Shape {
	private:
		const double radius;
		const double zMax, zMin, phiMax;
		const double thetaMax, thetaMin;
	public:
		Sphere(const Transform* LTW, const Transform* WTL, double radius, double zmax, double zmin, double phimax) :
			Shape(LTW, WTL), radius(radius), zMax(zmax), zMin(zmin), phiMax(phimax),
			thetaMax(acos(Clamp(zMin / radius, -1.f, 1.f))),
			thetaMin(acos(Clamp(zMax / radius, -1.f, 1.f))) {}
		virtual bool hit(const Ray& r_in, double min, double tMax)const;
		virtual bool intersect(const Ray& r_in, SurfaceInteraction& its, double tmin, double tmax)const;
		virtual Bound3f localBound()const;
		virtual Bound3f worldBound()const;
		virtual double area()const {
			return 4 * M_PI * radius;
		}
		virtual std::tuple<SurfaceInteraction, double> sample(const Point2f& rand)const;

		virtual std::tuple<SurfaceInteraction, double> sample(const SurfaceInteraction& inter,const Point2f& rand)const;

		virtual double pdf(const SurfaceInteraction& inter)const {
			return 1 / area();
		}

		virtual double pdf(const SurfaceInteraction& inter, const Vector3f& wi)const;
	};
}
#endif