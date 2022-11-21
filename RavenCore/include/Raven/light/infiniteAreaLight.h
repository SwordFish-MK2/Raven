#ifndef _RAVEN_LIGHT_INFINITE_AREA_LIGHT_H_
#define _RAVEN_LIGHT_INFINITE_AREA_LIGHT_H_

#include<Raven/core/base.h>
#include<Raven/core/light.h>
#include<Raven/core/mipmap.h>
#include<Raven/core/distribution.h>

namespace Raven {
	class InfiniteAreaLight :public Light {
	public:
		InfiniteAreaLight(const Transform* LightToWorld,const Transform* worldToLight,
			const Spectrum& L, int nSamples, const std::string& textureMap);
		//输入空间中的一个点p，在光源上随机采样，并计算出射的Radiance
		Spectrum sampleLi(const SurfaceInteraction& inter, const Point2f& uv, LightSample* lightSample)const;

		//返回光源向空间中辐射的总的能量
		virtual Spectrum power()const;
		
		//给定光源上的一个点与出射方向，计算采样的pdf
		virtual double pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const;

		Spectrum Le(const RayDifferential&)const;

		void preprocess(const Scene& scene);
	private:
		std::unique_ptr<Mipmap<RGBSpectrum>> lightMap;
		std::unique_ptr<Distribution2D> distribution;
		const Transform lightWorld;//用于调整image map的方向，决定哪个方向为上
		Point3f worldCenter;
		double worldRadius;
	};

		
}
#endif