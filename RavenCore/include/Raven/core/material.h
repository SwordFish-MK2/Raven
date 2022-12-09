#ifndef _RAVEN_CORE_MATERIAL_H_
#define _RAVEN_CORE_MATERIAL_H_

#include<Raven/core/base.h>
#include<Raven/core/bsdf.h>
#include<Raven/core/texture.h>
#include<Raven/core/object.h>

namespace Raven {
	class Material :public RavenObject {
	public:
		Material() {}
		virtual void computeScarttingFunctions(SurfaceInteraction& its, bool allowMultipleLobes)const = 0;
	};

	//class MaterialFactory {
	//	using MaterialConstructor = std::function<Ref<Material>(const PropertyList&)>;
	//public:
	//	static std::map<std::string,MaterialConstructor>
	//};
}
#endif