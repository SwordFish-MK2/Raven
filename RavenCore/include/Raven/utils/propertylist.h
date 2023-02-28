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

class PropertyList {
  enum class PointerType {
    Internal,
    External
  };

 public:
  PropertyList() : pNum(0), index(0) {}

  void setBoolean(const std::string& name, const bool& value);
  void setInteger(const std::string& name, const int& value);
  void setFloat(const std::string& name, const double& value);
  void setVector2f(const std::string& name, const Vector2f& value);
  void setPoint2f(const std::string& name, const Point2f& value);
  void setPoint3f(const std::string& name, const Point3f& value);
  void setVector3f(const std::string& name, const Vector3f& value);
  void setNormal3f(const std::string& name, const Normal3f& value);
  void setSpectra(const std::string& name, const Spectrum& value);
  void setString(const std::string& name, const std::string& value);
  void setSTexture(const std::string& name, Texture<Spectrum>* t);
  void setFTexture(const std::string& name, Texture<Float>* f);

  static void storeExternalPointer(const std::string& id, RavenObject* p);

  void setInternalPointer(RavenObject* p);
  void setExternalPointer(const std::string& id);

  // pasing in name and default value
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

  Texture<Float>*    getFTexture(const std::string& name);
  Texture<Spectrum>* getSTexture(const std::string& name);

  RavenObject* getObject(int);

  static void clear() {
    auto& refMap = getRefMap();
    refMap.erase(refMap.begin(), refMap.end());
  }

 private:
  static std::map<std::string, RavenObject*>& getRefMap() {
    static std::map<std::string, RavenObject*>
        refMap;  // 用于存放声明在外部的指针
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

  std::map<std::string, Property> propertyMap;

  std::map<std::string, Texture<Float>*>    ftMap;
  std::map<std::string, Texture<Spectrum>*> stMap;

  int pNum;  // 子对象的数目
  int index;
  std::vector<RavenObject*>
      pointerList;  // 用于存放直接声明在class内部的对象指针
};
}  // namespace Raven

// note:
// whenever parser generate any objects with id, puts them in the static
// map of PropertyList class, whenever an object is referenced by ref, get its
// point from map and store in the current propertylist object.
#endif