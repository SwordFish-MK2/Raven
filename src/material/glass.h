#ifndef _RAVEN_MATERIAL_GLASS_H_
#define _RAVEN_MATERIAL_GLASS_H_

#include"../core/base.h"
#include"../core/material.h"

#include"../shading_model/specular.h"

namespace Raven {

	class Glass final :public Material {
	public:
		Glass(
			const std::shared_ptr<Texture<Spectrum>>& kd,
			const std::shared_ptr<Texture<Spectrum>>& kt,
			const std::shared_ptr<Texture<double>>& uRough,
			const std::shared_ptr<Texture<double>>& vRough,
			const std::shared_ptr<Texture<double>>& bump = nullptr,
			double eta = 1.0) :
			kdTex(kd), ktTex(kt), uRough(uRough), vRough(vRough), bumpTex(bump), eta(eta) {}

			virtual void computeScarttingFunctions(SurfaceInteraction& its)const;
			
			static std::shared_ptr<Glass> build(const std::shared_ptr<Texture<Spectrum>>& kd,
				const std::shared_ptr<Texture<Spectrum>>& kt,
				const std::shared_ptr<Texture<double>>& uRough,
				const std::shared_ptr<Texture<double>>& vRough,
				const std::shared_ptr<Texture<double>>& bump = nullptr,
				double eta = 1.2) {
				return std::make_shared<Glass>(kd, kt, uRough, vRough, bump, eta);
			}
	private:
		std::shared_ptr<Texture<Spectrum>> kdTex;
		std::shared_ptr<Texture<Spectrum>> ktTex;
		std::shared_ptr<Texture<double>> uRough;
		std::shared_ptr<Texture<double>> vRough;
		std::shared_ptr<Texture<double>> bumpTex;
		double eta;

	};

}

#endif