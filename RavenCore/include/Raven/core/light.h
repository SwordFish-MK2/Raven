#ifndef _RAVEN_CORE_LIGHT_H_
#define _RAVEN_CORE_LIGHT_H_

#include<Raven/core/base.h>
#include<Raven/core/math.h>
#include<Raven/core/shape.h>
#include<Raven/core/spectrum.h>
#include<Raven/core/object.h>

namespace Raven {
	enum class LightFlag :int {
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
	class Light :public RavenObject {
	public:
		Light(const Ref<Transform>& LTW, const Ref<Transform>& WTL, int flag, int nSamples) :
			lightToWorld(LTW), worldToLight(WTL), flag(flag), nSamples(nSamples) {}

		//return radiance reached the given point emitted by light source, compute light incident direction and sampling pdf
		virtual Spectrum sampleLi(const SurfaceInteraction& inter, const Point2f& uv, LightSample* lightSample)const = 0;

		//return total power emitted by light source
		virtual Spectrum power()const = 0;

		virtual Spectrum Le(const RayDifferential& ray)const { return Spectrum(0.0); }

		virtual double pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const = 0;

		virtual void preprocess(const Scene& scene) {}

		virtual bool isDeltaLight() { return flag & (int)LightFlag::DeltaPosition || flag & (int)LightFlag::DeltaDirection; }

		int getSampleNumber()const { return nSamples; }

	protected:
		const int flag;					//whether light contines delta distribution
		const Ref<Transform> lightToWorld;
		const Ref<Transform> worldToLight;
		const int nSamples;				//n shadow rays prefered by the integrater
	};


	/// <summary>
	/// 面积光源虚基类
	/// </summary>
	class AreaLight :public Light {
	public:
		AreaLight(const Ref<Transform>& LTW, const Ref<Transform>& WTL, int flag, int nSamples, const Ref<Shape>& shape) :
			Light(LTW, WTL, flag, nSamples), shape_ptr(shape), area(shape->area()) {}

		//输入空间中的一个点p，在光源上随机采样，并计算出射的Radiance
		virtual Spectrum sampleLi(const SurfaceInteraction& inter, const Point2f& uv, LightSample* lightSample)const = 0;

		//给定光源上的一个点与出射方向，计算出射的Radiance
		virtual Spectrum Li(const SurfaceInteraction& inter, const Vector3f& wi)const = 0;

		//返回光源向空间中辐射的总的能量
		virtual Spectrum power()const = 0;

		//给定光源上的一个点与出射方向，计算采样的pdf
		virtual double pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const = 0;

		//virtual void setShapePtr(const Ref<Shape>& sptr) { shape_ptr = sptr; }
	protected:

		Ref<Shape> shape_ptr;
		const double area;
	};
}

#endif