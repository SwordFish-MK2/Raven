#ifndef _RAVEN_CORE_MATERIAL_H_
#define _RAVEN_CORE_MATERIAL_H_

#include<Raven/core/base.h>
#include<Raven/core/bsdf.h>
#include<Raven/core/texture.h>
#include<Raven/core/object.h>

namespace Raven {
#define _RAVEN_MATERIAL_REG_(regName,className,constructor)\
	class className##Reg{\
	private:\
		className##Reg(){\
			MaterialFactory::regClass(#regName,constructor);\
		}\
		static className##Reg regHelper;\
	};\

	class Material:public RavenObject {
	public:
		Material() {}
		virtual void computeScarttingFunctions(SurfaceInteraction& its)const = 0;
	};

	//class MaterialFactory {
	//	using MaterialConstructor = std::function<Ref<Material>(const PropertyList&)>;
	//public:
	//	static std::map<std::string,MaterialConstructor>
	//};
}
#endif