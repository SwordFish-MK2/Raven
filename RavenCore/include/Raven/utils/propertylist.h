#ifndef _RAVEN_UTILS_PROPERTYLIST_H_
#define _RAVEN_UTILS_PROPERTYLIST_H_

#include <Raven/core/base.h>
#include <Raven/core/math.h>
#include <Raven/core/spectrum.h>
#include <Raven/core/transform.h>

#include <optional>
#include <queue>

namespace Raven {

// 封装指针数据,only used inside the property list
struct Pointer {
 public:
  inline RavenObject* getRef() const { return my_pointer; }

  inline bool matchType(const std::string& type) { return type == my_type; }
  Pointer(const std::string& type, RavenObject* p, bool byRef = false)
      : my_type(type), my_pointer(p), ref(byRef) {}

 private:
  std::string  my_type;     // 指针指向的对象类型
  RavenObject* my_pointer;  // 指针
  bool         ref;         // 是否包含id
};

class PropertyList {
  enum class RefType {
    Ref,
    RefById
  };

 public:
  PropertyList() : refCount(0) {}

  void setBoolean(const std::string&, const bool& value);
  void setInteger(const std::string&, const int& value);
  void setFloat(const std::string&, const double& value);
  void setVector2f(const std::string&, const Vector2f& value);
  void setPoint2f(const std::string&, const Point2f& value);
  void setPoint3f(const std::string&, const Point3f& value);
  void setVector3f(const std::string&, const Vector3f& value);
  void setNormal3f(const std::string&, const Normal3f& value);
  void setSpectra(const std::string&, const Spectrum& value);
  void setString(const std::string&, const std::string& value);
  void setPointer(const std::string& type, const Pointer& ref);

  static void setPointerById(const std::string& refid,
                             const std::string& type,
                             RavenObject*       ref,
                             PropertyList&      currentPropertyList);

  bool        getBoolean(const std::string&, const bool&) const;
  int         getInteger(const std::string&, const int&) const;
  double      getFloat(const std::string&, const double&) const;
  Point2f     getPoint2f(const std::string&, const Point2f&) const;
  Vector2f    getVector2f(const std::string&, const Vector2f&) const;
  Point3f     getPoint3f(const std::string&, const Point3f&) const;
  Vector3f    getVector3f(const std::string&, const Vector3f&) const;
  Normal3f    getNormal3f(const std::string&, const Normal3f&) const;
  Spectrum    getSpectra(const std::string&, const Spectrum&) const;
  std::string getString(const std::string&, const std::string&) const;

  Pointer getObjectRef(int) const;

  static void clear() {
    auto& refMap = getRefMap();
    refMap.erase(refMap.begin(), refMap.end());
  }

 private:
  static std::map<std::string, Pointer>& getRefMap() {
    static std::map<std::string, Pointer> refMap;  // 用于存放声明在外部的指针
    return refMap;
  }

  struct Property {
    Property() : type(boolean_type) {}

    enum Type {
      boolean_type,
      integer_type,
      float_type,
      vector2f_type,
      point2f_type,
      point3f_type,
      vector3f_type,
      normal_type,
      spectra_type,
      matrix_type,
      string_type,
      ref_type,
      object_type
    } type;

    struct Value {
      bool        boolean_value;
      int         integer_value;
      double      float_value;
      Vector2f    vector2f_value;
      Point2f     point2f_value;
      Point3f     point3f_value;
      Vector3f    vector3f_value;
      Normal3f    normal_value;
      Spectrum    spectra_value;
      std::string string_value;
    } value;
  };
  int refCount;

  std::map<std::string, Property> propertyMap;
  std::vector<Pointer> refQueue;  // 用于存放直接声明在class内部的对象指针
  std::vector<RefType>     refTypeList;
  std::vector<std::string> refIds;
};
}  // namespace Raven
#endif