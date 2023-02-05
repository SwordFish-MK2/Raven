#ifndef _RAVEN_CORE_OBJECT_H_
#define _RAVEN_CORE_OBJECT_H_

#include <Raven/core/base.h>

#include <functional>
#include <string>

namespace Raven {

class RavenObject {
protected:
  virtual ~RavenObject() {}

private:
};

class RavenClass {
public:
  using ObjectConstructor =
      std::function<Ref<RavenObject>(const PropertyList& properties)>;

  RavenClass(const ObjectConstructor& cons) : constructor(cons) {}

  // 构建对象
  virtual Ref<RavenObject> constructObject() const;

private:
  std::string       type;
  ObjectConstructor constructor;
};
}  // namespace Raven

#endif