#ifndef _RAVEN_CORE_LIGHT_H_
#define _RAVEN_CORE_LIGHT_H_

#include"base.h"
#include"math.h"
#include"interaction.h"
#include"scene.h"
#include"spectrum.h"

namespace Raven {
	enum LightFlag :int {
		DeltaPosition = 1, DeltaDirection = 2, AreaLight = 4, InfiniteLight = 8
	};

	struct LightSample {
		Point3f p;
		Vector3f wi;
		double pdf;
		Normal3f n;
	};

	/// <summary>
	/// 光源类接口，所有光源必须继承该类
	/// </summary>
	class Light {
	public:
		Light(const Transform* LTW, const Transform* WTL, int flag, int nSamples) :
			lightToWorld(LTW), worldToLight(WTL), flag(flag), nSamples(nSamples) {}
		//return radiance reached the given point emitted by light source, compute light incident direction and sampling pdf
		virtual Spectrum sampleLi(const SurfaceInteraction& inter, const Point2f& uv, LightSample* lightSample)const = 0;
		virtual Spectrum Li(const SurfaceInteraction& inter, const Vector3f& wi)const = 0;
		//return total power emitted by light source
		virtual Spectrum power()const = 0;
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
}

#endif