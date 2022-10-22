#include"propertylist.h"
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
		auto & prop=propertyMap[name];												\
		prop.value.xmlTypeName##_value=value;										\
		prop.type=Property::xmlTypeName##_type;										\
	}																				\
	Type PropertyList::get##TypeName(const std::string& name)const{					\
		auto& it=propertyMap.find(name);											\
		return it->second.value.xmlTypeName##_value;								\
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