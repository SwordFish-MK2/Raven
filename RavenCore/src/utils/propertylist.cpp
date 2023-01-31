#include<Raven/utils/propertylist.h>
namespace Raven {
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


	void PropertyList::setObjectRef(
		const std::string& type,
		const Ref<RavenObject>& ref
	) {
		refTypeList.push_back(RefType::Ref);
		refCount++;
		ObjectRef objref(type, ref);
		refQueue.push_back(objref);
	}

	void PropertyList::setObjectRefById(
		const std::string& refId,
		const std::string& type,
		const Ref<RavenObject>& ref,
		PropertyList& list) {
		list.refTypeList.push_back(RefType::RefById);
		list.refCount++;
		ObjectRef objref(type, ref);
		auto& refMap = getRefMap();
		refMap[refId] = objref;
	}

	ObjectRef PropertyList::getObjectRef(int n)const {
		if (n > refCount) {
			return ObjectRef("Failed to get Raven object ref, ref vector overflow.", nullptr);
		}
		int refC = 0;
		int refByIdC = 0;

		for (int i = 0; i < n; i++) {
			if (refTypeList[i] == RefType::Ref)
				refC++;
			else
				refByIdC++;
		}

		if (refTypeList[n] == RefType::Ref) {
			return refQueue[refC];
		}
		else {
			std::string refId = refIds[refByIdC];
			auto& refMap = getRefMap();
			const auto& it = refMap.find(refId);
			if (it == refMap.end()) {
				std::string msg("Failed to parse ref id :");
				return ObjectRef(msg + refId, nullptr);
			}
			return it->second;
		}
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