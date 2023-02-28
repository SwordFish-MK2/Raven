#include <Raven/utils/propertylist.h>

#include <cstdlib>

#include "Raven/utils/log.h"

namespace Raven {
#define DEFINE_PROPERTY_ACCESSOR(Type, TypeName, xmlTypeName)                  \
  void PropertyList::set##TypeName(const std::string& name,                    \
                                   const Type&        value) {                        \
    if (propertyMap.find(name) != propertyMap.end()) {                         \
      std::cout << "Property has been set." << std::endl;                      \
    }                                                                          \
    auto& prop                     = propertyMap[name];                        \
    prop.value.xmlTypeName##_value = value;                                    \
    prop.type                      = Property::xmlTypeName##_type;             \
  }                                                                            \
  Type PropertyList::get##TypeName(const std::string& name,                    \
                                   const Type&        defaultVal) const {             \
    const auto& it = propertyMap.find(name);                                   \
    if (it != propertyMap.end())                                               \
      return it->second.value.xmlTypeName##_value;                             \
    else                                                                       \
      return defaultVal;                                                       \
  }

void PropertyList::setInternalPointer(RavenObject* p) {
  pointerList.push_back(p);
}
void PropertyList::setExternalPointer(const std::string& id) {
  const auto& refMap = getRefMap();
  const auto& pIt    = refMap.find(id);

  // no pointer matches or id is empty
  if (pIt == refMap.end()) {
    RERROR("Parsing xml failed, unable to set ref with id {0}", id);
    exit(EXIT_FAILURE);
  }

  // set pointer
  pointerList.push_back(pIt->second);
}

void PropertyList::storeExternalPointer(const std::string& id, RavenObject* p) {
  auto&       refMap = getRefMap();
  const auto& pIt    = refMap.find(id);

  //test if there is a pointer with same id
  if (pIt != refMap.end()) {
    RERROR("Parsing xml failed, ref id {0} duplicated", id);
    exit(EXIT_FAILURE);
  }

  //set pointer
  refMap[id] = p;
}

RavenObject* PropertyList::getObject(int n) {
  if (index >= n)
    return nullptr;
  return pointerList[index++];
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
}  // namespace Raven