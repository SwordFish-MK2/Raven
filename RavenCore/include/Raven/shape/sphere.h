#ifndef _RAVEN_SHAPE_SPHERE_H_
#define _RAVEN_SHAPE_SPHERE_H_


#include<Raven/core/base.h>
#include<Raven/core/shape.h>
#include<Raven/utils/propertylist.h>
#include<Raven/utils/factory.h>
namespace Raven {
	class Sphere final :public Shape {

	public:
		Sphere(const Ref<Transform>& LTW, const Ref<Transform>& WTL, double radius) :
			Shape(LTW, WTL), radius(radius) {}

		bool hit(const Ray& r_in)const override;

		bool intersect(const Ray& r_in, HitInfo& info)const override;

		SurfaceInteraction getGeoInfo(const Point3f& hitInfo)const override;

		Bound3f localBound()const override;

		Bound3f worldBound()const override;

		double area()const override { return 4 * M_PI * radius; }

		std::tuple<SurfaceInteraction, double> sample(const Point2f& rand)const override;

		std::tuple<SurfaceInteraction, double> sample(const SurfaceInteraction& inter, const Point2f& rand)const override;

		double pdf(const SurfaceInteraction& inter)const override { return 1 / area(); }

		double pdf(const SurfaceInteraction& inter, const Vector3f& wi)const override;

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