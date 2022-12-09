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

		void computeScarttingFunctions(SurfaceInteraction& its,
			bool allowMultipleLobes) const override;

		static std::shared_ptr<Mirror> build(
			const std::shared_ptr<Texture<Spectrum>>& r,
			const std::shared_ptr<Texture<double>>& bump = nullptr) {
			return std::make_shared<Mirror>(r, bump);
		}

		static Ref<Material> construct(const PropertyList& param) {
			const auto& r = std::dynamic_pointer_cast<Texture<Spectrum>>(param.getObjectRef(0).getRef());
			//const auto& bump=std::dynamic_pointer_cast<Texture<double>>(param.getObjectRef(1).getRef());

			return std::make_shared<Mirror>(r, nullptr);
		}

	private:
		std::shared_ptr<Texture<Spectrum>> rTex;
		std::shared_ptr<Texture<double>> bumpTex;
	};

	_RAVEN_CLASS_REG_(mirror,Mirror,Mirror::construct)
}


#endif