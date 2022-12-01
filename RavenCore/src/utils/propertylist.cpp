#include<Raven/utils/propertylist.h>
namespace Raven {

	//void PropertyList::setPointer(const std::string& id, RavenPointerType type) {
	//	pointerMap[id] = type;
	//}

	//RavenPointerType PropertyList::getPointer(const std::string& id)const {
	//	auto item = pointerMap.find(id);
	//	return item->second;
	//}

#define DEFINE_PROPERTY_ACCESSOR(Type,TypeName,xmlTypeName)							\
	void PropertyList::set##TypeName(const std::string& name,const Type& value){	\
		if(propertyMap.find(name)!=propertyMap.end()){								\
			std::cout<<"Property has been set."<<std::endl;							\
		}																			\
		auto & prop=propertyMap[name];										\
		prop.value.xmlTypeName##_value=value;										\
		prop.type=Property::xmlTypeName##_type;										\
	}																				\
	Type PropertyList::get##TypeName(const std::string& name,const Type& defaultVal)const{					\
		const auto & it=propertyMap.find(name);											\
		if(it!=propertyMap.end())\
			return it->second.value.xmlTypeName##_value;								\
		else\
			return defaultVal;	\
	}	

	std::map<std::string, ObjectRef> PropertyList::refMap;

	void PropertyList::setObjectRef(
		const std::string& type, 
		const Ref<RavenObject>& ref) {
		ObjectRef objref(type, ref);
		refQueue.push_back(objref);
	}

	void PropertyList::setObjectRefById(
		const std::string& refId,
		const std::string& type,
		const Ref<RavenObject>& ref) {
		ObjectRef objref(type, ref);
		refMap[refId] = objref;
	}

	ObjectRef PropertyList::getObjectRef(int n)const {
		if (refQueue.size() <= n) {
			return ObjectRef("nullptr", nullptr);
		}
		ObjectRef ref = refQueue[n];
		return ref;
	}

	ObjectRef PropertyList::getObjectRefById(const std::string& refId) {
		const auto& it = refMap.find(refId);
		if (it == refMap.end()) {
			return ObjectRef("nullptr", nullptr);
		}
		else
			return it->second;
	}


	DEFINE_PROPERTY_ACCESSOR(bool, Boolean, boolean)

		DEFINE_PROPERTY_ACCESSOR(int, Integer, integer)

		DEFINE_PROPERTY_ACCESSOR(double, Float, float)

		DEFINE_PROPERTY_ACCESSOR(Point2f, Point2f, point2f)
		DEFINE_PROPERTY_ACCESSOR(Vector2f, Vector2f, vector2f)
		DEFINE_PROPERTY_ACCESSOR(Point3f, Point3f, point3f)
		DEFINE_PROPERTY_ACCESSOR(Vector3f, Vector3f, vector3f)
		DEFINE_PROPERTY_ACCESSOR(Normal3f, Normal3f, normal)
		DEFINE_PROPERTY_ACCESSOR(Spectrum, Spectra, spectra)
		DEFINE_PROPERTY_ACCESSOR(std::string, String, string)
}