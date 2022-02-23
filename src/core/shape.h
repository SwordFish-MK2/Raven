#ifndef _RAVEN_CORE_SHAPE_H_
#define _RAVEN_CORE_SHAPE_H_

#include"base.h"
#include"math.h"
#include"interaction.h"
#include"transform.h"
#include"aabb.h"
#include"ray.h"

namespace Raven {
	/// <summary>
/// Shape interface, all geometrics must inherit this class
/// </summary>
	class Shape {
	protected:
		const Transform* localToWorld;
		const Transform* worldToLocal;	
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			Shape(const Transform* LTW, const Transform* WTL) :localToWorld(LTW), worldToLocal(WTL) {}
		//intersect incident ray with shape and return whether shape is hitted
		virtual bool hit(const Ray& r_in, double tmin, double tMax)const = 0;
		//intersect incident ray with shape and compute surfaceIntersection 
		virtual bool intersect(const Ray& r_in, SurfaceInteraction& its, double tmin, double tmax)const = 0;
		//return the bounding box of shape in local space
		virtual Bound3f localBound()const = 0;
		//return the bounding box of shape in world space
		virtual Bound3f worldBound()const = 0;
		//return area of shape surface
		virtual double area()const = 0;

		//pdf of uniformly sample shapeing
		virtual double pdf(const SurfaceInteraction&)const { return 1 / area(); }
		//uniformly sample shape surface acorrding to uv coordinate 
		virtual SurfaceInteraction sample(const Point2f& uv)const = 0;
		//pdf of sample visiable direction 
		virtual double pdf(const SurfaceInteraction& inter, const Vector3f& wi)const {
			//intersect sample ray to light geometry
			Point3f origin = inter.p;
			Ray r(origin, wi);
			SurfaceInteraction lightInter;
			if (!intersect(r, lightInter, 1e-6, std::numeric_limits<double>::max()))
				return 0;
			//convert the pdf from integral of light surface to integral of the solid angle of sample point
			double pdf = DistanceSquared(lightInter.p, inter.p) / (std::abs(Dot(lightInter.n, -wi)) * area());
			return pdf;
		}
		//sample visiable surface from given point
		virtual SurfaceInteraction sample(const SurfaceInteraction& ref, const Point2f& uv)const { return sample(uv); }
	};
}

#endif