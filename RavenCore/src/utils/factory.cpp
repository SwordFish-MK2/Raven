#include<Raven/utils/factory.h>

namespace Raven {

	void Factory::regClass(
		const std::string& className,
		const Constructor& constructor)
	{
		classMap[className] = constructor;
	}

	bool Factory::registed(const std::string& className)const
	{
		auto it = classMap.find(className);
		if (it == classMap.end()) {
			return false;
		}
		return true;
	}

	Ref<RavenObject> Factory::generate(
		const std::string& className,
		const PropertyList& param)const
	{
		auto it = classMap.find(className);
		Ref<RavenObject> cons = it->second(param);
		return cons;
	}

}