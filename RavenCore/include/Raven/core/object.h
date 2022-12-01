#ifndef _RAVEN_CORE_OBJECT_H_
#define _RAVEN_CORE_OBJECT_H_

#include<string>
#include<functional>
#include<Raven/core/base.h>

namespace Raven {




	template<typename T>
	using Ref = std::shared_ptr<T>;

	class RavenObject {
	protected:
		~RavenObject() {}
	};

	class RavenClass {
	public:
		using ObjectConstructor = std::function<Ref<RavenObject>(const PropertyList& properties)>;

		RavenClass(const ObjectConstructor& cons) :constructor(cons) {}
		
		//构建对象
		virtual Ref<RavenObject> constructObject()const;
	private:
		
		std::string type;
		ObjectConstructor constructor;
	};
}

#endif 