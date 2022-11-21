#ifndef _RAVEN_SHAPE_SPHERE_H_
#define _RAVEN_SHAPE_SPHERE_H_


#include<Raven/core/base.h>
#include<Raven/core/shape.h>
#include<Raven/utils/propertylist.h>

namespace Raven {
	class Sphere :public Shape {
	private:
		const double radius;
	public:
		Sphere(const Transform* LTW, const Transform* WTL, double radius) :
			Shape(LTW, WTL), radius(radius) {}
		virtual bool hit(const Ray& r_in, double tMax = FLT_MAX)const;

		virtual bool intersect(const Ray& r_in, HitInfo& info, double tMax = FLT_MAX)const;

		virtual SurfaceInteraction getGeoInfo(const Point3f& hitInfo)const;

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

	inline std::shared_ptr<Sphere> makeSphereShape(
		const std::shared_ptr<Transform>& LTW,
		const std::shared_ptr<Transform>& WTL,
		const PropertyList& pList) {
		double radius = pList.getFloat("radius");
		return std::make_shared<Sphere>(LTW.get(), WTL.get(), radius);
	}
}
#endif