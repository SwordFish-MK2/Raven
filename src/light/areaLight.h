#ifndef _RAVEN_LIGHT_AREA_LIGHT_H_
#define _RAVEN_LIGHT_AREA_LIGHT_H_


#include"../core/base.h"
#include"../core/light.h"

namespace Raven {

	/// <summary>
	/// 面积光源虚基类
	/// </summary>
	class AreaLight :public Light {
	public:
		AreaLight(const Transform* LTW, const Transform* WTL, int flag, int nSamples, const Shape* shape) :
			Light(LTW, WTL, flag, nSamples), shape_ptr(shape), area(shape->area()) {}

		////输入空间中的一个点p，在光源上随机采样，并计算出射的Radiance
		virtual Spectrum sampleLi(const SurfaceInteraction& inter, const Point2f& uv, LightSample* lightSample)const = 0;

		//给定光源上的一个点与出射方向，计算出射的Radiance
		virtual Spectrum Li(const SurfaceInteraction& inter, const Vector3f& wi)const = 0;

		//返回光源向空间中辐射的总的能量
		virtual Spectrum power()const = 0;

		//给定光源上的一个点与出射方向，计算采样的pdf
		virtual double pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const = 0;

		//virtual Vector3f sample_Li(const SurfaceInteraction& inter, const Point2f& uv,
		//	Vector3f* wi, double* pdf, SurfaceInteraction* lightSample)const = 0;
	protected:

		const Shape* shape_ptr;
		const double area;
	};


	/// <summary>
	/// DiffuseAreaLight实现类，该光源上的任意一点向空间中的任意方向辐射同等大小的Radiance
	/// </summary>
	class DiffuseAreaLight :public AreaLight {
	public:
		DiffuseAreaLight(
			const Transform* LTW,
			const Transform* WTL,
			int nSamples,
			const Shape* shape,
			const Spectrum& I) :
			AreaLight(LTW, WTL, LightFlag::AreaLight, nSamples, shape), emittedRadiance(I) {}

		virtual Spectrum Li(const SurfaceInteraction& p, const Vector3f& wi)const;

		virtual Spectrum sampleLi(const SurfaceInteraction& inter, const Point2f& uv,
			LightSample* lightSample)const;

		virtual Spectrum power()const;

		virtual double pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const;
	private:
		const Spectrum emittedRadiance;
	};

	inline std::shared_ptr<DiffuseAreaLight> makeDiffuseAreaLight(
		const std::shared_ptr<Transform>& LTW,
		const std::shared_ptr<Transform>& WTL,
		const std::shared_ptr<Shape>& shape,
		const PropertyList& pList) {
		int nSamples = pList.getInteger("nsamples");
		Spectrum emit = pList.getSpectra("emit");
		return std::make_shared<DiffuseAreaLight>(LTW.get(), WTL.get(), nSamples, shape.get(), emit);
	}
}


#endif 