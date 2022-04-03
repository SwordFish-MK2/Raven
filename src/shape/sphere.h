#ifndef _RAVEN_SHAPE_SPHERE_H_
#define _RAVEN_SHAPE_SPHERE_H_


#include"../core/base.h"
#include"../core/shape.h"

namespace Raven {
	class Sphere :public Shape {
	private:
		const double radius;
	public:
		Sphere(const Transform* LTW, const Transform* WTL, double radius) :
			Shape(LTW, WTL), radius(radius) {}
		virtual bool hit(const Ray& r_in, double tMax)const;

		virtual std::optional<SurfaceInteraction> intersect(const Ray& r_in, double tMax)const;

		virtual Bound3f localBound()const;
		
		virtual Bound3f worldBound()const;
		
		virtual double area()const {
			return 4 * M_PI * radius;
		}
		
		virtual std::tuple<SurfaceInteraction, double> sample(const Point2f& rand)const;

		virtual std::tuple<SurfaceInteraction, double> sample(const SurfaceInteraction& inter, const Point2f& rand)const;

		virtual double pdf(const SurfaceInteraction& inter)const {
			return 1 / area();
		}

		virtual double pdf(const SurfaceInteraction& inter, const Vector3f& wi)const;

		static std::shared_ptr<Sphere> build(const Transform* LTW, const Transform* WTL,
			double radius);
	};
}
#endif