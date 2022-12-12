#ifndef _RAVEN_UTILS_FACTORY_H_
#define _RAVEN_UTILS_FACTORY_H_

#include<Raven/core/base.h>
#include<Raven/core/object.h>

namespace Raven {

	class Factory {

		using Constructor = std::function<Ref<RavenObject>(const PropertyList&)>;

	public:
		void regClass(const std::string&, const Constructor&);

		Ref<RavenObject> generate(const std::string&, const PropertyList&)const;

		bool registed(const std::string&)const;

		static Factory& getInstance() {
			static Factory instance;
			return instance;
		}
	private:
		std::map<std::string, Constructor> classMap;
	};

#define _RAVEN_CLASS_REG_(regName,className,constructor)\
	class className##Reg{\
	private:\
		className##Reg(){\
			Factory& facInstance=Factory::getInctance();\
			facInstance.regClass(#regName,constructor);\
		}\
		static className##Reg regHelper;\
	};

}

#endif