#ifndef _RAVEN_CORE_MATERIAL_H_
#define _RAVEN_CORE_MATERIAL_H_

#include"base.h"
#include"bsdf.h"
#include"texture.h"

namespace Raven {

	class Material {
	public:
		Material() {}
		virtual void computeScarttingFunctions(SurfaceInteraction& its)const = 0;
	};
}
#endif