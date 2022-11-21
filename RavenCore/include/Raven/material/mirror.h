#ifndef _RAVEN_MATERIAL_MIRROR_H_
#define _RAVEN_MATERIAL_MIRROR_H_

#include<Raven/core/material.h>
#include<Raven/core/base.h>
#include<Raven/core/spectrum.h>
#include<Raven/core/texture.h>
#include<Raven/core/distribution.h>

namespace Raven {

	class Mirror final : public Material {
	public:
		Mirror(
			const std::shared_ptr<Texture<Spectrum>>& r,
			const std::shared_ptr<Texture<double>>& bump = nullptr) :
			rTex(r), bumpTex(bump) {}

		void computeScarttingFunctions(SurfaceInteraction& its) const;

		static std::shared_ptr<Mirror> build(
			const std::shared_ptr<Texture<Spectrum>>& r, 
			const std::shared_ptr<Texture<double>>& bump = nullptr) {
			return std::make_shared<Mirror>(r, bump);
		}
	private:
		std::shared_ptr<Texture<Spectrum>> rTex;
		std::shared_ptr<Texture<double>> bumpTex;
	};
	inline std::shared_ptr<Mirror> makeMirrorMaterial(
		const std::shared_ptr<Texture<Spectrum>>& r,
		const PropertyList& pList) {
		return std::make_shared<Mirror>(r, nullptr);
	}
}


#endif