#ifndef _RAVEN_CORE_LIGHT_H_
#define _RAVEN_CORE_LIGHT_H_

#include"base.h"
#include"math.h"
#include"interaction.h"
#include"scene.h"

namespace Raven {
	enum LightFlag :int {
		DeltaPosition = 1, DeltaDirection = 2, AreaLight = 4, InfiniteLight = 8
	};

	/// <summary>
	/// 光源类接口，所有光源必须继承该类
	/// </summary>
	class Light {
	public:
		Light(const Transform* LTW, const Transform* WTL, int flag, int nSamples) :
			lightToWorld(LTW), worldToLight(WTL), flag(flag), nSamples(nSamples) {}
		//return radiance reached the given point emitted by light source, compute light incident direction and sampling pdf
		virtual Vector3f sample_Li(const SurfaceInteraction& inter, const Point2f& uv, Vector3f* wi,
			double* pdf, SurfaceInteraction* lightSample)const = 0;
		virtual Vector3f Li(const SurfaceInteraction& inter, const Vector3f& wi)const = 0;
		//return total power emitted by light source
		virtual Vector3f power()const = 0;
		virtual double pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const = 0;
		virtual void preprocess(const Scene& scene) {}
		virtual bool isDeltaLight() { return flag & DeltaPosition || flag & DeltaDirection; }

		int getSampleNumber()const { return nSamples; }
	protected:

		const int flag;					//whether light contines delta distribution
		const Transform* lightToWorld;
		const Transform* worldToLight;
		const int nSamples;				//n shadow rays prefered by the integrater
	};

	///// <summary>
	///// Class for testing whether given light sample is visiable from the receiving point
	///// </summary>
	//class VisibilityTester {
	//private:
	//	SurfaceInteraction p0, p1;//two end points of shadow ray to be traced in world space
	//public:
	//	VisibilityTester(const SurfaceInteraction& po, const SurfaceInteraction& p1) :
	//		p0(p0), p1(p1) {}
	//	bool testVisitbility(const Scene& scene)const {
	//		Vector3f direction = Normalize(p1.p - p0.p);
	//		Ray testRay(p0.p, direction);
	//		SurfaceInteraction lightInter;
	//		if (!scene.intersect(testRay, lightInter, 1e-6, FLT_MAX))
	//			return true;
	//	}
	//};
}

#endif