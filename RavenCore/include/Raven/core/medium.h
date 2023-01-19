#ifndef _RAVEN_CORE_MEDIA_H_
#define _RAVEN_CORE_MEDIA_H_

#include<Raven/core/base.h>
#include<Raven/core/math.h>
#include<Raven/core/spectrum.h>
#include<Raven/core/object.h>

namespace Raven {
	/// <summary>
	/// ���ʱ߽�Ľӿڣ���������������ʵ�ָ��
	///</summary>
	struct MediumInterface {
		//interface
		MediumInterface(
			const std::shared_ptr<Medium>& medium
		) :outside(medium), inside(medium) {}

		MediumInterface(
			const std::shared_ptr<Medium>& outside,
			const std::shared_ptr<Medium>& inside) :
			outside(outside), inside(inside) {}

		MediumInterface() :outside(nullptr), inside(nullptr) {}

		//data
		Ref<Medium> outside;
		Ref<Medium> inside;
	};
	
	/// <summary>
	/// PhaseFunction�ӿ�
	/// </summary>
	class PhaseFunction {
	public:
		//����Phase Function ��ֵ
		virtual double p(const Vector3f& wi, const Vector3f& wo)const = 0;
	};

	/// <summary>
	/// ��Henyey Greenstein����ĸ���ͬ��phase funcionģ��
	/// </summary>
	class HenyeyGreensteinPhaseFunction :public PhaseFunction {
	public:
		double p(const Vector3f& wi, const Vector3f& wo)const override;
	private:
		double g;
	};

	/// <summary>
	/// ���ʽӿ�
	/// </summary>
	class Medium {
	public:
		//���贫��ú����Ĺ����ڴ���������δ���ڵ�������ȫ��Χ�ڵ�ǰ�����У�������ߵ�beam transmittance
		virtual Spectrum Tr(const Ray& ray, Sampler& sampler)const = 0;
	};

	inline double PhaseHG(double cosTheta, double g) {
		double inv4PI = 0.25 * M_PI;
		double denom = 1 + g * g + 2 * g * (cosTheta);
		return inv4PI * (1 - g * g) / denom * sqrt(denom);
	}


}
#endif