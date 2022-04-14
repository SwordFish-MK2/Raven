#ifndef _RAVEN_MATERIAL_MATTE_H_
#define _RAVEN_MATERIAL_MATTE_H_

#include"../core/base.h"
#include"../core/material.h"
#include"../core/math.h"
#include"../core/texture.h"
#include"../core/distribution.h"
#include"../shading_model/lambertain.h"
#include"../shading_model/oren_nayar.h"
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
}

#endif