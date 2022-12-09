#ifndef _RAVEN_SHAPE_SPHERE_H_
#define _RAVEN_SHAPE_SPHERE_H_


#include<Raven/core/base.h>
#include<Raven/core/shape.h>
#include<Raven/utils/propertylist.h>
#include<Raven/utils/factory.h>
namespace Raven {
	class Sphere :public Shape {

	public:
		Sphere(const Ref<Transform>& LTW, const Ref<Transform>& WTL, double radius) :
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

		static Ref<Shape> construct(const PropertyList& param) {
			ObjectRef ltwRef = param.getObjectRef(0);
			const Ref<Transform>& ltw = std::dynamic_pointer_cast<Transform>(ltwRef.getRef());
			const Ref<Transform> wtl = std::make_shared<Transform>(Inverse(*ltw));
			double radius = param.getFloat("radius", 1.0);
			return std::make_shared<Sphere>(ltw, wtl, radius);
		}

	private:
		const double radius;
	};

	_RAVEN_CLASS_REG_(sphere, Sphere, Sphere::construct)
}
#endif