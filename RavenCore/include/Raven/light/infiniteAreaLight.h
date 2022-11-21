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
		//����ռ��е�һ����p���ڹ�Դ���������������������Radiance
		Spectrum sampleLi(const SurfaceInteraction& inter, const Point2f& uv, LightSample* lightSample)const;

		//���ع�Դ��ռ��з�����ܵ�����
		virtual Spectrum power()const;
		
		//������Դ�ϵ�һ��������䷽�򣬼��������pdf
		virtual double pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const;

		Spectrum Le(const RayDifferential&)const;

		void preprocess(const Scene& scene);
	private:
		std::unique_ptr<Mipmap<RGBSpectrum>> lightMap;
		std::unique_ptr<Distribution2D> distribution;
		const Transform lightWorld;//���ڵ���image map�ķ��򣬾����ĸ�����Ϊ��
		Point3f worldCenter;
		double worldRadius;
	};

		
}
#endif