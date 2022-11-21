#ifndef _RAVEN_MATERIAL_MATTE_H_
#define _RAVEN_MATERIAL_MATTE_H_

#include<Raven/core/base.h>
#include<Raven/core/material.h>
#include<Raven/core/math.h>
#include<Raven/core/texture.h>
#include<Raven/core/distribution.h>
#include<Raven/shading/lambertain.h>
#include<Raven/shading/oren_nayar.h>
namespace Raven {

	class MatteMaterial :public Material {
	private:
		std::shared_ptr<Texture<double>> sigma;//surface roughness value 
		std::shared_ptr<Texture<Spectrum>> kd;//diffuse reflection value
		std::shared_ptr<Texture<double>> bump;//bump function
	public:
		MatteMaterial(
			const std::shared_ptr<Texture<double>>& sigma,
			const std::shared_ptr<Texture<Spectrum>>& Kd,
			const std::shared_ptr<Texture<double>>& bump = NULL)
			:sigma(sigma), kd(Kd), bump(bump) {}

		void computeScarttingFunctions(SurfaceInteraction& its) const;


		static std::shared_ptr<MatteMaterial> buildConst(double sigma, const Spectrum& kd);

		static std::shared_ptr<MatteMaterial> build(
			const std::shared_ptr<Texture<double>>& sigma,
			const std::shared_ptr<Texture<Spectrum>>& kd,
			const std::shared_ptr<Texture<double>>& bump = nullptr) {
			return std::make_shared<MatteMaterial>(sigma, kd, bump);
		}
	};

	inline std::shared_ptr<MatteMaterial> makeMatteMaterial(
		const std::shared_ptr<Texture<double>>& sigma,
		const std::shared_ptr<Texture<Spectrum>>& kd,
		const PropertyList& property) {
		return std::make_shared<MatteMaterial>(sigma, kd, nullptr);
	}
}

#endif