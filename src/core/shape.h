#ifndef _RAVEN_CORE_SHAPE_H_
#define _RAVEN_CORE_SHAPE_H_

#include"base.h"
#include"math.h"

#include"transform.h"
#include"aabb.h"
#include"ray.h"
#include<optional>
namespace Raven {
	enum PrimType {
		PTriangle, PSphere
	};

	struct HitInfo {
		double hitTime;
		Vector3f wo;
		Point3f pHit;

		HitInfo() {}

		void setInfo(const Point3f& p,double t,const Vector3f dir) {
			pHit = p;
			hitTime = t;
			wo = dir;
		}
	};

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
		virtual bool hit(const Ray& r_in, double tMax)const = 0;

		//intersect incident ray with shape and compute surfaceIntersection 
		virtual bool intersect(const Ray& r_in, HitInfo& hitInfo, double tMax)const = 0;

		virtual SurfaceInteraction getGeoInfo(const Point3f& hitInfo)const = 0;

		//return the bounding box of shape in local space
		virtual Bound3f localBound()const = 0;

		//return the bounding box of shape in world space
		virtual Bound3f worldBound()const = 0;

		//return area of shape surface
		virtual double area()const = 0;

		//���ȵ��ڼ����������������ؼ���������һ���㣬pdfΪ��������ֵ�pdf
		virtual std::tuple<SurfaceInteraction, double> sample(const Point2f& rand)const = 0;

		//�����ռ��е�һ���㣬�ڼ�������������һ���㣬pdfΪ������ǵĻ���
		virtual std::tuple<SurfaceInteraction, double> sample(const SurfaceInteraction& ref, const Point2f& rand)const;

		//�ڼ������Ͼ��Ȳ�����pdf�����������
		virtual double pdf(const SurfaceInteraction&)const { return 1 / area(); }

		//�ڿռ��и����������pdf��������ǻ���
		virtual double pdf(const SurfaceInteraction& inter, const Vector3f& wi)const;
	};
}

#endif