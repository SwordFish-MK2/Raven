#ifndef _RAVEN_CORE_MATERIAL_H_
#define _RAVEN_CORE_MATERIAL_H_

#include<Raven/core/base.h>
#include<Raven/core/bsdf.h>
#include<Raven/core/texture.h>

namespace Raven {

	class Material {
	public:
		Material() {}
		virtual void computeScarttingFunctions(SurfaceInteraction& its)const = 0;
	};
}
#endif