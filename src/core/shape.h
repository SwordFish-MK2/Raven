#ifndef _RAVEN_CORE_SHAPE_H_
#define _RAVEN_CORE_SHAPE_H_

#include"base.h"
#include"math.h"
#include"interaction.h"
#include"transform.h"
#include"aabb.h"
#include"ray.h"
#include<optional>
namespace Raven {
	/// <summary>
	/// Shape interface, all geometrics must inherit this class
	/// </summary>
	class Shape {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	protected:
		const Transform* localToWorld;
		const Transform* worldToLocal;
	public:
		Shape(const Transform* LTW, const Transform* WTL) :localToWorld(LTW), worldToLocal(WTL) {}

		//intersect incident ray with shape and return whether shape is hitted
		virtual bool hit(const Ray& r_in, double tmin, double tMax)const = 0;

		//intersect incident ray with shape and compute surfaceIntersection 
		virtual std::optional<SurfaceInteraction> intersect(const Ray& r_in, double tmin, double tmax)const = 0;

		//return the bounding box of shape in local space
		virtual Bound3f localBound()const = 0;

		//return the bounding box of shape in world space
		virtual Bound3f worldBound()const = 0;

		//return area of shape surface
		virtual double area()const = 0;

		//均匀的在几何体表面采样，返回几何体表面的一个点，pdf为对面积积分的pdf
		virtual std::tuple<SurfaceInteraction, double> sample(const Point2f& rand)const = 0;

		//给定空间中的一个点，在几何体体表面采样一个点，pdf为对立体角的积分
		virtual std::tuple<SurfaceInteraction, double> sample(const SurfaceInteraction& ref, const Point2f& rand)const;

		//在几何体上均匀采样的pdf，对面积积分
		virtual double pdf(const SurfaceInteraction&)const { return 1 / area(); }

		//在空间中给定点采样的pdf，对立体角积分
		virtual double pdf(const SurfaceInteraction& inter, const Vector3f& wi)const;
	};
}

#endif