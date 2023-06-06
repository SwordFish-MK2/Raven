#include <Raven/core/object.h>
#include <Raven/utils/factory.h>

#include <memory>


namespace Raven {

void Factory::regClass(const std::string& className,
                       const Constructor& constructor) {
  classMap[className] = constructor;
}

bool Factory::registed(const std::string& className) const {
  auto it = classMap.find(className);
  if (it == classMap.end()) {
    return false;
  }
  return true;
}

RavenObject* Factory::generate(const std::string&  name,
                               const PropertyList& property) const {
  auto         it   = classMap.find(name);
  RavenObject* cons = it->second(property);
  return cons;
}

}  // namespace Raven