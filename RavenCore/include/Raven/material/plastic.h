#ifndef _RAVEN_MATERIAL_PLASTIC_H_
#define _RAVEN_MATERIAL_PLASTIC_H_

#include<Raven/core/material.h>
#include<Raven/core/base.h>
#include<Raven/shading/microfacet.h>
#include<Raven/core/texture.h>

namespace Raven {
	class Plastic :public Material {
	public:
		Plastic(
			const std::shared_ptr<Texture<Spectrum>>& kdTexture,
			const std::shared_ptr<Texture<Spectrum>>& ksTexture,
			const std::shared_ptr<Texture<double>>& roughnessTexture, 
			const std::shared_ptr<Texture<double>>& bumpTexture = nullptr) :
			kd(kdTexture),
			ks(ksTexture),
			roughness(roughnessTexture),
			bump(bumpTexture) {}

		virtual void computeScarttingFunctions(SurfaceInteraction& its)const;

		static std::shared_ptr<Plastic> build(
			const std::shared_ptr<Texture<Spectrum>>& kd,
			const std::shared_ptr<Texture<Spectrum>>& ks,
			const std::shared_ptr<Texture<double>>& roughness,
			const std::shared_ptr<Texture<double>>& bump = nullptr);
	private:
		std::shared_ptr<Texture<Spectrum>> kd;
		std::shared_ptr<Texture<Spectrum>> ks;
		std::shared_ptr<Texture<double>> roughness;
		std::shared_ptr<Texture<double>> bump;


	};
	inline std::shared_ptr<Plastic> makePlasticMaterial(
		const std::shared_ptr<Texture<Spectrum>>& kd,
		const std::shared_ptr<Texture<Spectrum>>& ks,
		const std::shared_ptr<Texture<double>>& roughness,
		const PropertyList& pList) {
		return std::make_shared<Plastic>(kd, ks, roughness, nullptr);
	}
}

#endif