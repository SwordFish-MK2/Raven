#include <Raven/camera/projectiveCamera.h>
#include <Raven/core/film.h>
#include <Raven/core/integrator.h>
#include <Raven/core/object.h>
#include <Raven/core/shape.h>
#include <Raven/core/spectrum.h>
#include <Raven/core/texture.h>
#include <Raven/core/transform.h>
#include <Raven/light/areaLight.h>
#include <Raven/material/glass.h>
#include <Raven/material/matte.h>
#include <Raven/material/mirror.h>
#include <Raven/material/plastic.h>
#include <Raven/shape/mesh.h>
#include <Raven/shape/sphere.h>
#include <Raven/textute/imageTexture.h>
#include <Raven/textute/mapping.h>
#include <Raven/textute/solidTexture.h>
#include <Raven/utils/factory.h>
#include <Raven/utils/loader.h>
#include <Raven/utils/log.h>
#include <Raven/utils/parser.h>
#include <tinyxml2.h>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "Raven/core/primitive.h"

#define ParserErorr(x) std::cerr << x << std::endl

// #define _RAVEN_PARSE_CLASS_TYPE_(myType, ClassType)                            \
//   const tinyxml2::XMLAttribute* typeAttr = node->FindAttribute("type");        \
//   if (!typeAttr) {                                                             \
//     std::cout << "No " << #myType << " type is specified, Raven abort.\n";     \
//     return;                                                                    \
//   } else {                                                                     \
//     const auto& factory = Factory::getInstance();                              \
//     std::string type    = typeAttr->Value();                                   \
//     if (factory.registed(type) == false) {                                     \
//       std::cout << "Unknown parameter is passed to " << #myType                \
//                 << "type, Raven abort.\n ";                                    \
//       return;                                                                  \
//     }                                                                          \
//     Ref<ClassType> my_class =                                                  \
//         std::dynamic_pointer_cast<ClassType>(factory.generate(type, list));    \
//     my_##myType = my_class;                                                    \
//     break;                                                                     \
//   }

// #define _RAVEN_PASER_SCENE_CLASS_TYPE_(myType, ClassType)                      \
//   const tinyxml2::XMLAttribute* typeAttr = node->FindAttribute("type");        \
//   if (!typeAttr) {                                                             \
//     std::cout << "No " << #myType << " type is specified, Raven abort.\n";     \
//     return;                                                                    \
//   }                                                                            \
//   const tinyxml2::XMLAttribute* idAttr = node->FindAttribute("id");            \
//   if (!idAttr) {                                                               \
//   } else {                                                                     \
//     std::string type       = typeAttr->Value();                                \
//     auto        myType##It = myType##Map.find(type);                           \
//     if (myType##It == myType##Map.end()) {                                     \
//       std::cout << "Unknown parameter is passed to " << #myType                \
//                 << " type,Raven abort.\n  ";                                   \
//       return;                                                                  \
//     }                                                                          \
//     Raven##ClassType##Type ClassType##Tag = myType##It->second;                \
//     std::pair<Raven##ClassType##Type, PropertyList> myProperty =               \
//         std::make_pair(ClassType##Tag, list);                                  \
//     myType##Property = myProperty;                                             \
//     break;                                                                     \
//   }

namespace Raven {

bool Parser::parse(const std::string& file) {
  Ref<Film>       my_film;
  Ref<Camera>     my_camera;
  Ref<Integrator> my_integrator;
  enum RTag {
    // classes
    RScene,
    RFilm,
    RCamera,
    RIntegrator,
    RTexture,
    RMaterial,
    RShape,
    REmit,
    RPrimitive,
    RMapping,
    RTransform,
    RMedium,
    RPhase,
    RFilter,
    RSampler,
    RVolume,

    // proprerties
    RPropertyStart,
    RBoolean,
    RInteger,
    RFloat,
    RString,
    RVector2,
    RVector3,
    RPoint2,
    RPoint3,
    RSpectra,
    RSpectraRGB,
    RNormal,
    RPointer,
    RMatrix
  };

  // 创建从xml字符串到Raven数据结构tag之间的映射
  std::map<std::string, RTag> mapping;
  mapping["scene"]      = RTag::RScene;
  mapping["bsdf"]       = RTag::RMaterial;
  mapping["emitter"]    = RTag::REmit;
  mapping["film"]       = RTag::RFilm;
  mapping["integrator"] = RTag::RIntegrator;
  mapping["medium"]     = RTag::RMedium;
  mapping["phase"]      = RTag::RPhase;
  mapping["filter"]     = RTag::RFilter;
  mapping["sampler"]    = RTag::RSampler;
  mapping["sensor"]     = RTag::RCamera;
  mapping["shape"]      = RTag::RShape;
  mapping["texture"]    = RTag::RTexture;
  mapping["volume"]     = RTag::RVolume;

  mapping["spectrum"] = RTag::RSpectra;
  mapping["ref"]      = RTag::RPointer;
  mapping["rbg"]      = RTag::RSpectraRGB;
  mapping["string"]   = RTag::RString;
  mapping["boolean"]  = RTag::RBoolean;
  mapping["int"]      = RTag::RInteger;
  mapping["float"]    = RTag::RFloat;
  mapping["vector2"]  = RTag::RVector2;
  mapping["vector3"]  = RTag::RVector3;
  mapping["matrix"]   = RTag::RMatrix;
  mapping["point2"]   = RTag::RPoint2;
  mapping["point3"]   = RTag::RPoint3;
  mapping["normal"]   = RTag::RNormal;

  // 用于处理xml的函数，该函数每次处理xml的一个节点
  std::function<void(const tinyxml2::XMLElement*, PropertyList*,
                     const tinyxml2::XMLElement*)>
      parseTag = [&](const tinyxml2::XMLElement* node, PropertyList* pList,
                     const tinyxml2::XMLElement* fatherNode = nullptr) {
        // 获取当前节点的名字，在mapping中查找是否有对应的映射
        auto it  = mapping.find(node->Name());
        int  tag = it->second;

        // 没有找到对应的映射，当前的节点名称不合法
        if (it == mapping.end()) {
          std::cout << "Unknown class name " << node->Name() << ",Raven abort."
                    << std::endl;
          return;
        }

        // 判断当前节点对应的类型
        bool isScene  = tag == RTag::RScene;
        bool isObject = tag < RTag::RPropertyStart && tag != RTag::RScene;

        // 当前节点对应场景类
        if (isScene) {
          // 检查scene节点的attribute，查看用户是否指定了场景使用的加速结构
          const tinyxml2::XMLAttribute* accelerationAttribute =
              node->FindAttribute("accelerate");

          // 用户在场景定义文件中指定了场景的加速结构，使用指定的加速结构构建场景类
          if (accelerationAttribute) {
            std::string accType = node->FindAttribute("accelerate")->Value();
            if (accType == "bvh") {
              accelerate = AccelType::BVH;
            } else if (accType == "kdtree") {
              accelerate = AccelType::KdTree;
            } else if (accType == "list") {
              accelerate = AccelType::List;
            } else {
              std::cout << "Unknown parameter " << accType
                        << " is passed to scene acceleration type.\n";
              accelerate = AccelType::BVH;
            }
          }

          // 用户未指定加速结构，默认使用BVH加速场景
          else
            accelerate = AccelType::BVH;

          // 场景节点的子结点中定义了场景中的图元，遍历所有子节点获取图元信息
          const tinyxml2::XMLElement* childNode = node->FirstChildElement();
          while (childNode) {
            parseTag(childNode, nullptr, nullptr);
            childNode = childNode->NextSiblingElement();
          }
        }

        // 当前节点对应Raven中的类
        if (isObject) {
          // 创建propertylist用于存储构造该类对象所需的数据
          PropertyList list;

          // 类节点的自节点中为该类的数据或子成员，遍历xml子节点获取这些数据存储在propertylist中
          const tinyxml2::XMLElement* childNode = node->FirstChildElement();
          while (childNode) {
            parseTag(childNode, &list, node);
            childNode = childNode->NextSiblingElement();
          }

          // 在获取了该类所有的子成员与数据后，确定该节点具体对应的类型，将节点类型与存有数据的PropertyList绑定
          switch (tag) {
            // 构建幕布
            case RFilm: {
              if (fatherNode == nullptr ||
                  fatherNode->Name() != std::string("camera")) {
                RERROR("Parsing xml failed, {0} must be declared inside {1}",
                       "film", "camera");
              }

              // 获取幕布类型
              const tinyxml2::XMLAttribute* typeAttr =
                  node->FindAttribute("type");

              // 在xml中未指定类型
              if (!typeAttr) {
                RERROR("Parsing xml failed, {0} type unset", "film");
                return;
              }

              // 成功获取幕布类型
              else {
                // 获取工厂实例
                const auto& factory = Factory::getInstance();

                std::string type = typeAttr->Value();  // 类型对应的string

                // 在xml中指定的对象类型未在工厂中注册（即指定了raven不支持的对象
                if (factory.registed(type) == false) {
                  RERROR("Parsing xml failed, defined unsupported {0} class",
                         "film");
                  exit(EXIT_FAILURE);
                }

                // 取得幕布指针
                RavenObject* f = factory.generate(type, list);
                pList->setInternalPointer(f);
                break;
              }
            }

            // 构建相机
            case RCamera: {
              // 获取相机类型
              if (fatherNode == nullptr ||
                  fatherNode->Name() != std::string("integrator")) {
                RERROR("Parsing xml failed, {0} must be declared inside {1}",
                       "camera", "integrator");
                exit(EXIT_FAILURE);
              }

              // get camera type
              const tinyxml2::XMLAttribute* typeAttr =
                  node->FindAttribute("type");
              if (!typeAttr) {
                RERROR("Parsing xml failed, {0} type unset", "camera");
                exit(EXIT_FAILURE);
              }

              // 成功获取幕布类型
              else {
                // 获取相机工厂实例
                const auto& factory = Factory::getInstance();

                std::string type = typeAttr->Value();  // 类型对应的string

                // 在xml中指定的对象类型未在工厂中注册（即指定了raven不支持的对象
                if (factory.registed(type) == false) {
                  RERROR("Parsing xml failed, defined unsupported {0} class",
                         "film");
                  exit(EXIT_FAILURE);
                }

                // success
                RavenObject* p = factory.generate(type, list);
                pList->setInternalPointer(p);
                break;
              }
            }

            case RSampler: {
              // 获取相机类型
              if (fatherNode == nullptr ||
                  fatherNode->Name() != std::string("integrator")) {
                RERROR("Parsing xml failed, {0} must be declared inside {1}",
                       "sampler", "integrator");
                exit(EXIT_FAILURE);
              }

              // get camera type
              const tinyxml2::XMLAttribute* typeAttr =
                  node->FindAttribute("type");
              if (!typeAttr) {
                RERROR("Parsing xml failed, {0} type unset", "sampler");
                exit(EXIT_FAILURE);
              }

              // 成功获取幕布类型
              else {
                // 获取工厂实例
                const auto& factory = Factory::getInstance();

                std::string type = typeAttr->Value();  // 类型对应的string

                // 在xml中指定的对象类型未在工厂中注册（即指定了raven不支持的对象
                if (factory.registed(type) == false) {
                  RERROR(
                      "Parsing xml failed, defined unsupported {0} class: "
                      "{1}",
                      "sampler", "type");
                  exit(EXIT_FAILURE);
                }

                // success
                RavenObject* p = factory.generate(type, list);
                pList->setInternalPointer(p);
                break;
              }
            }

            case RIntegrator: {
              // 获取相机类型
              if (fatherNode == nullptr ||
                  fatherNode->Name() != std::string("integrator")) {
                RERROR("Parsing xml failed, {0} must be declared inside {1}",
                       "camera", "integrator");
                exit(EXIT_FAILURE);
              }

              // get object type
              const tinyxml2::XMLAttribute* typeAttr =
                  node->FindAttribute("type");
              if (!typeAttr) {
                RERROR("Parsing xml failed, {0} type unset", "camera");
                exit(EXIT_FAILURE);
              }

              // successfully get object type
              else {
                // get factory instance
                const auto& factory = Factory::getInstance();

                std::string type = typeAttr->Value();  // 类型对应的string

                // 在xml中指定的对象类型未在工厂中注册（即指定了raven不支持的对象
                if (factory.registed(type) == false) {
                  RERROR(
                      "Parsing xml failed, defined unsupported {0} class: "
                      "{1}",
                      "integrator", type);
                  exit(EXIT_FAILURE);
                }

                // success
                Integrator* p    = (Integrator*)factory.generate(type, list);
                this->integrator = std::unique_ptr<Integrator>(p);
                break;
              }
            }

            // TODO::finish texture
            // referring to solid texture and image texture, (const textures
            // are not included)
            case RTexture: {
              // get type
              const tinyxml2::XMLAttribute* typeAttr =
                  node->FindAttribute("type");
              const tinyxml2::XMLAttribute* nameAttr =
                  node->FindAttribute("name");

              // type is not set
              if (!typeAttr) {
                RERROR("Parsing xml failed, {0} type unset", "texture");
                exit(EXIT_FAILURE);
              }
              if (!nameAttr) {
                RERROR("Parsing xml failed,{0} name unset", "texture");
                exit(EXIT_FAILURE);
              }

              // 生成纹理

              const std::string& type    = typeAttr->Value();
              const auto&        factory = Factory::getInstance();
              if (factory.registed(type) == false) {
                RERROR("Parsing xml failed, defined unsupported {0} class",
                       "film");
                exit(EXIT_FAILURE);
              }

              std::string        name = nameAttr->Value();
              Texture<Spectrum>* p =
                  (Texture<Spectrum>*)factory.generate(type, list);

              pList->setSTexture(name, p);
              break;
            }

            //
            case RMaterial: {
              const tinyxml2::XMLAttribute* typeAttr =
                  node->FindAttribute("type");
              if (!typeAttr) {
                RERROR("Parsing xml failed, {0} type unset", "material");
                exit(EXIT_FAILURE);
              }
              std::string type = typeAttr->Value();

              // get factory instance and generate object
              const auto& factory = Factory::getInstance();
              if (!factory.registed(type)) {
                RERROR("Parsing xml failed, declared unsupported {0} class {1}",
                       "material", type);
                exit(EXIT_FAILURE);
              }
              RavenObject* my_object = factory.generate(type, list);

              // get id
              bool        external = fatherNode->Name() == std::string("scene");
              const auto& idAttr   = node->FindAttribute("id");
              if (external && !idAttr) {
                RERROR(
                    "Parsing xml failed, defining {0} under scene node "
                    "without assigning id",
                    "material");
                exit(EXIT_FAILURE);
              } else if (!external && idAttr) {
                RERROR(
                    "Parsing xml failed, can not assign id while defing {0} "
                    "inside class",
                    "material");
                exit(EXIT_FAILURE);
              } else if (external && idAttr) {
                std::string id = idAttr->Value();
                list.storeExternalPointer(id, my_object);
              } else {
                list.setInternalPointer(my_object);
              }
              break;
            }

            case RShape: {
              // get type
              const tinyxml2::XMLAttribute* typeAttr =
                  node->FindAttribute("type");
              if (!typeAttr) {
                RERROR("Parsing xml failed, {0} type unset", "shape");
                exit(EXIT_FAILURE);
              }
              std::string type = typeAttr->Value();

              // get factory instance and generate object
              const auto& factory = Factory::getInstance();
              if (!factory.registed(type)) {
                RERROR("Parsing xml failed, declared unsupported {0} class {1}",
                       "material", type);
                exit(EXIT_FAILURE);
              }

              // pointer of primitive
              Shape* p = (Shape*)factory.generate(type, list);

              // std::shared_ptr<Primitive>
              // sharedp=std::make_shared<Primitive>(p);
              break;
            }

            case REmit: {
              const tinyxml2::XMLAttribute* typeAttr =
                  node->FindAttribute("type");
              if (!typeAttr) {
                RERROR("Parsing xml failed, {0} type unset", "emitter");
                exit(EXIT_FAILURE);
              }
              std::string type = typeAttr->Value();

              // get factory instance and generate object
              const auto& factory = Factory::getInstance();
              if (!factory.registed(type)) {
                RERROR("Parsing xml failed, declared unsupported {0} class {1}",
                       "emitter", type);
                exit(EXIT_FAILURE);
              }
              RavenObject* my_object = factory.generate(type, list);

              // get id
              bool        external = fatherNode->Name() == std::string("scene");
              const auto& idAttr   = node->FindAttribute("id");
              if (external && !idAttr) {
                RERROR(
                    "Parsing xml failed, defining {0} under scene node "
                    "without assigning id",
                    "emitter");
                exit(EXIT_FAILURE);
              } else if (!external && idAttr) {
                RERROR(
                    "Parsing xml failed, can not assign id while defing {0} "
                    "inside class",
                    "emitter");
                exit(EXIT_FAILURE);
              } else if (external && idAttr) {
                std::string id = idAttr->Value();
                list.storeExternalPointer(id, my_object);
              } else {
                list.setInternalPointer(my_object);
              }
              break;
            }

            case RTransform: {
              break;
            }
          }
        }

        else {
          // current node is raven property, insert its value to property
          // list
          switch (tag) {
            case RBoolean: {
              std::string name  = node->FindAttribute("name")->Value();
              bool        value = node->FindAttribute("value")->BoolValue();
              pList->setBoolean(name, value);
              break;
            }
            case RInteger: {
              std::string name  = node->FindAttribute("name")->Value();
              int         value = node->FindAttribute("value")->IntValue();
              pList->setInteger(name, value);
              break;
            }
            case RFloat: {
              std::string name  = node->FindAttribute("name")->Value();
              double      value = node->FindAttribute("value")->DoubleValue();
              pList->setFloat(name, value);
              break;
            }
            case RString: {
              std::string name  = node->FindAttribute("name")->Value();
              std::string value = node->FindAttribute("value")->Value();
              pList->setString(name, value);
              break;
            }
            case RVector2: {
              std::string name = node->FindAttribute("name")->Value();
              const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
              Vector2f                      value = toVector2f(v->Value());
              pList->setVector2f(name, value);
              break;
            }
            case RVector3: {
              std::string name = node->FindAttribute("name")->Value();
              const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
              Vector3f                      value = toVector3f(v->Value());
              pList->setVector3f(name, value);
              break;
            }
            case RPoint3: {
              std::string name = node->FindAttribute("name")->Value();
              const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
              Point3f                       value = toPoint3f(v->Value());
              pList->setPoint3f(name, value);
              break;
            }
            case RPoint2: {
              std::string name = node->FindAttribute("name")->Value();
              const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
              Point2f                       value = toPoint2f(v->Value());
              pList->setPoint2f(name, value);
              break;
            }
            case RNormal: {
              std::string name = node->FindAttribute("name")->Value();
              const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
              Normal3f                      value = toNormal(v->Value());
              pList->setNormal3f(name, value);
              break;
            }
            case RSpectraRGB: {
              // 获取rgb的值
              const auto& nameAttr = node->FindAttribute("name");
              const auto& vstrAttr = node->FindAttribute("value");
              if (!nameAttr) {
                RERROR("Parsing xml failed, rgb name unset");
                exit(EXIT_FAILURE);
              }
              if (!vstrAttr) {
                RERROR("Parsing xml failed,rgb value unset");
                exit(EXIT_FAILURE);
              }
              std::string name = nameAttr->Value();
              std::string vstr = vstrAttr->Value();

              Vector3f rgbValue = toVector3f(vstr);
              Spectrum value    = Spectrum::fromRGB(rgbValue);
              pList->setSpectra(name, value);
              break;
            }
            case RSpectra: {
              // const auto& nameAttr  = node->FindAttribute("name");
              // const auto& valueAttr = node->FindAttribute("value");
              // if (!nameAttr) {
              //   RERROR("Parsing xml failed,spectra name unset");
              //   exit(EXIT_FAILURE);
              // }
              // if (!valueAttr) {
              //   RERROR("Parsing xml failed,spectra value unset");
              //   exit(EXIT_FAILURE);
              // }
              // std::string name    = nameAttr->Value();
              // std::string vString = valueAttr->Value();

              // std::shared_ptr<double[]> lambda;

              // std::shared_ptr<double[]> spectrumValue;
              // int                       n;

              // getSpectrumSample(lambda, spectrumValue, &n, vString);
              // Spectrum sampledSpectra =
              //     Spectrum::fromSampled(&lambda[0], &spectrumValue[0], n);

              // pList->setSpectra(name, sampledSpectra);
              RINFO("{0}", "Spectra unsupported");

              break;
            }

            // set pointer
            case RPointer: {
              const tinyxml2::XMLAttribute* idAttr = node->FindAttribute("id");
              if (!idAttr) {
                RERROR("Parsing xml failed, ref id undefined");
                exit(EXIT_FAILURE);
              }
              std::string id = idAttr->Value();
              pList->setExternalPointer(id);
              break;
            }
          }
        }
      };

  const char*           fileName = file.c_str();
  tinyxml2::XMLDocument doc;
  doc.LoadFile(fileName);

  tinyxml2::XMLElement* root = doc.RootElement();
  if (root) {
    std::cout << root->Name() << std::endl;
    tinyxml2::XMLElement* child = root->FirstChildElement();
    while (child) {
      parseTag(child, nullptr, nullptr);
      child = child->NextSiblingElement();
    }
  }
  return true;
}

std::shared_ptr<RavenRenderingPrograme> Parser::makePrograme() const {
  std::vector<std::shared_ptr<Transform>>                       ts;
  std::vector<std::shared_ptr<TriangleMesh>>                    meshes;
  std::vector<std::shared_ptr<Primitive>>                       primitiveList;
  std::vector<std::shared_ptr<Light>>                           sceneLights;
  std::map<std::string, std::shared_ptr<Transform>>             transform;
  std::map<std::string, std::shared_ptr<Texture<Spectrum>>>     spectrumTexture;
  std::map<std::string, std::shared_ptr<Texture<double>>>       floatTexture;
  std::map<std::string, std::shared_ptr<Material>>              material;
  std::map<std::string, std::shared_ptr<Shape>>                 shape;
  std::map<std::string, std::shared_ptr<Light>>                 light;
  std::map<std::string, std::shared_ptr<Primitive>>             primitive;
  std::map<std::string, std::shared_ptr<TextureMapping2D>>      mapping;
  std::map<std::string, std::vector<std::shared_ptr<Triangle>>> meshList;
  std::map<std::string, std::vector<std::shared_ptr<Light>>>    lightList;
  std::map<std::string, std::vector<std::shared_ptr<Primitive>>> plm;

  generate film auto    ft = filmProperty.first;
  std::shared_ptr<Film> film;
  if (ft == RavenFilmType::NoFilter) {
    PropertyList ppt = filmProperty.second;
    film             = makeFilm(ppt);
  }

  generate camera auto    ct = cameraProperty.first;
  std::shared_ptr<Camera> camera;
  if (ct == RavenCameraType::RPerspective) {
    generate camera world PropertyList ppt         = cameraProperty.second;
    Point3f                            pos         = ppt.getPoint3f("pos");
    Point3f                            lookat      = ppt.getPoint3f("lookat");
    Vector3f                           up          = ppt.getVector3f("up");
    Transform                          cameraWorld = LookAt(pos, lookat, up);
    Transform                          raster = Raster(film->yRes, film->xRes);
    camera = makePerspectiveCamera(cameraWorld, raster, ppt);
  } else if (ct == RavenCameraType::ROrthographic) {
    PropertyList ppt         = cameraProperty.second;
    Point3f      pos         = ppt.getPoint3f("pos");
    Point3f      lookat      = ppt.getPoint3f("lookat");
    Vector3f     up          = ppt.getVector3f("up");
    Transform    cameraWorld = LookAt(pos, lookat, up);
    Transform    raster      = Raster(film->yRes, film->xRes);
    camera                   = makeOrthographicCamera(cameraWorld, raster, ppt);
  }

  generate integrator auto  rt = integratorProperty.first;
  std::shared_ptr<Renderer> renderer;
  if (rt == RavenRendererType::RPath) {
    PropertyList ppt = integratorProperty.second;
    renderer         = makePathTracingRenderer(film, camera, ppt);
  }

  // generate scene

  // generate transform
  for (size_t i = 0; i < transformProperty.size(); i++) {
    auto [id, tt, ppt] = transformProperty[i];
    if (tt == RavenTransformType::RIdentity) {
      std::shared_ptr<Transform> trans =
          std::make_shared<Transform>(Identity());
      if (ppt.pointerList.size() != 0) {
        for (size_t j = 0; j < ppt.pointerList.size(); j++) {
          std::string                tId = ppt.pointerList[j].id;
          auto                       tIt = transform.find(tId);
          std::shared_ptr<Transform> t   = tIt->second;
          *trans                         = (*t) * (*trans);
        }
      }
      transform[id] = trans;
      ts.push_back(trans);
    } else if (tt == RavenTransformType::RRotate) {
      double                     angle = ppt.getFloat("angle");
      Vector3f                   axis  = ppt.getVector3f("axis");
      std::shared_ptr<Transform> trans =
          std::make_shared<Transform>(Rotate(angle, axis));
      if (ppt.pointerList.size() != 0) {
        for (size_t j = 0; j < ppt.pointerList.size(); j++) {
          std::string                tId = ppt.pointerList[j].id;
          auto                       tIt = transform.find(tId);
          std::shared_ptr<Transform> t   = tIt->second;
          *trans                         = (*t) * (*trans);
        }
      }
      transform[id] = trans;
      ts.push_back(trans);
    } else if (tt == RavenTransformType::RRotateX) {
      double                     angle = ppt.getFloat("angle");
      std::shared_ptr<Transform> trans =
          std::make_shared<Transform>(RotateX(angle));
      if (ppt.pointerList.size() != 0) {
        for (size_t j = 0; j < ppt.pointerList.size(); j++) {
          std::string                tId = ppt.pointerList[j].id;
          auto                       tIt = transform.find(tId);
          std::shared_ptr<Transform> t   = tIt->second;
          *trans                         = (*t) * (*trans);
        }
      }
      transform[id] = trans;
      ts.push_back(trans);
    } else if (tt == RavenTransformType::RRotateY) {
      double                     angle = ppt.getFloat("angle");
      std::shared_ptr<Transform> trans =
          std::make_shared<Transform>(RotateY(angle));
      if (ppt.pointerList.size() != 0) {
        for (size_t j = 0; j < ppt.pointerList.size(); j++) {
          std::string                tId = ppt.pointerList[j].id;
          auto                       tIt = transform.find(tId);
          std::shared_ptr<Transform> t   = tIt->second;
          *trans                         = (*t) * (*trans);
        }
      }
      transform[id] = trans;
      ts.push_back(trans);
    } else if (tt == RavenTransformType::RRotateZ) {
      double                     angle = ppt.getFloat("angle");
      std::shared_ptr<Transform> trans =
          std::make_shared<Transform>(RotateZ(angle));
      if (ppt.pointerList.size() != 0) {
        for (size_t j = 0; j < ppt.pointerList.size(); j++) {
          std::string                tId = ppt.pointerList[j].id;
          auto                       tIt = transform.find(tId);
          std::shared_ptr<Transform> t   = tIt->second;
          *trans                         = (*t) * (*trans);
        }
      }
      transform[id] = trans;
      ts.push_back(trans);
    } else if (tt == RavenTransformType::RScale) {
      Vector3f                   vector = ppt.getVector3f("scale");
      std::shared_ptr<Transform> trans =
          std::make_shared<Transform>(Scale(vector));
      if (ppt.pointerList.size() != 0) {
        for (size_t j = 0; j < ppt.pointerList.size(); j++) {
          std::string                tId = ppt.pointerList[j].id;
          auto                       tIt = transform.find(tId);
          std::shared_ptr<Transform> t   = tIt->second;
          *trans                         = (*t) * (*trans);
        }
      }
      transform[id] = trans;
      ts.push_back(trans);
    } else if (tt == RavenTransformType::RTranslate) {
      Vector3f                   vector = ppt.getVector3f("translate");
      std::shared_ptr<Transform> trans =
          std::make_shared<Transform>(Translate(vector));
      if (ppt.pointerList.size() != 0) {
        for (size_t j = 0; j < ppt.pointerList.size(); j++) {
          std::string                tId = ppt.pointerList[j].id;
          auto                       tIt = transform.find(tId);
          std::shared_ptr<Transform> t   = tIt->second;
          *trans                         = (*t) * (*trans);
        }
      }
      transform[id] = trans;
      ts.push_back(trans);
    } else if (tt == RavenTransformType::RInverse) {
      std::string                tId = ppt.pointerList[0].id;
      auto                       tIt = transform.find(tId);
      std::shared_ptr<Transform> t   = tIt->second;
      std::shared_ptr<Transform> trans =
          std::make_shared<Transform>(t->inverse());
      transform[id] = trans;
      ts.push_back(trans);
    }
  }

  // generate mapping
  for (size_t i = 0; i < mappingProperty.size(); i++) {
    auto [id, mt, ppt] = mappingProperty[i];
    if (mt == RavenMappingType::RUVMapping) {
      mapping[id] = makeUVMapping(ppt);
    } else if (mt == RavenMappingType::RSphereMapping) {
      std::string transformId = ppt.pointerList[0].id;
      auto        transformIt = transform.find(transformId);

      mapping[id] = makeSphericalMapping2D(transformIt->second, ppt);
    }
  }

  // generate texture
  for (size_t i = 0; i < textureProperty.size(); i++) {
    auto [id, tt, ppt] = textureProperty[i];
    if (tt == RavenTextureType::RConstFloat) {
      floatTexture[id] = makeConstTextureFloat(ppt);
    } else if (tt == RavenTextureType::RConstSpectrum) {
      spectrumTexture[id] = makeConstTextureSpectrum(ppt);
    } else if (tt == RavenTextureType::RCheckerboardFloat) {
      get pointer std::string oddId     = ppt.pointerList[0].id;
      std::string             evenId    = ppt.pointerList[1].id;
      std::string             mappingId = ppt.pointerList[2].id;
      auto                    oddIt     = floatTexture.find(oddId);
      auto                    evenIt    = floatTexture.find(evenId);
      auto                    mappingIt = mapping.find(mappingId);

      floatTexture[id] = makeCheckeredFloat(oddIt->second, evenIt->second,
                                            mappingIt->second, ppt);
    } else if (tt == RavenTextureType::RCheckerboardSpectrum) {
      std::string oddId     = ppt.pointerList[0].id;
      std::string evenId    = ppt.pointerList[1].id;
      std::string mappingId = ppt.pointerList[2].id;
      auto        oddIt     = spectrumTexture.find(oddId);
      auto        evenIt    = spectrumTexture.find(evenId);
      auto        mappingIt = mapping.find(mappingId);

      spectrumTexture[id] = makeCheckeredSpectrum(oddIt->second, evenIt->second,
                                                  mappingIt->second, ppt);
    } else if (tt == RavenTextureType::RImageMapFloat) {
      std::string mappingId = ppt.pointerList[0].id;
      auto        mappingIt = mapping.find(mappingId);
      floatTexture[id]      = makeImageTextureFloat(mappingIt->second, ppt);
    } else if (tt == RavenTextureType::RImageMapSpectrum) {
      std::string mappingId = ppt.pointerList[0].id;
      auto        mappingIt = mapping.find(mappingId);
      spectrumTexture[id]   = makeImageTextureSpectrum(mappingIt->second, ppt);
    }
  }

  // generate material
  for (size_t i = 0; i < materialProperty.size(); i++) {
    auto [id, mt, ppt] = materialProperty[i];
    if (mt == RavenMaterialType::RMatte) {
      std::string spectrumId = ppt.pointerList[0].id;
      std::string floatId    = ppt.pointerList[1].id;
      auto        kdIt       = spectrumTexture.find(spectrumId);
      auto        sigmaIt    = floatTexture.find(floatId);

      std::shared_ptr<Texture<Spectrum>> kd    = kdIt->second;
      std::shared_ptr<Texture<double>>   sigma = sigmaIt->second;

      material[id] = makeMatteMaterial(sigma, kd, ppt);
    } else if (mt == RavenMaterialType::RMirror) {
      std::string rId = ppt.pointerList[0].id;
      auto        rIt = spectrumTexture.find(rId);

      std::shared_ptr<Texture<Spectrum>> r = rIt->second;
      material[id]                         = makeMirrorMaterial(r, ppt);
    } else if (mt == RavenMaterialType::RGlass) {
      std::string kdId = ppt.pointerList[0].id;
      std::string ktId = ppt.pointerList[1].id;
      std::string uId  = ppt.pointerList[2].id;
      std::string vId  = ppt.pointerList[3].id;

      auto kdIt = spectrumTexture.find(kdId);
      auto ktIt = spectrumTexture.find(ktId);
      auto uIt  = floatTexture.find(uId);
      auto vIt  = floatTexture.find(vId);

      std::shared_ptr<Texture<Spectrum>> kd = kdIt->second;
      std::shared_ptr<Texture<Spectrum>> kt = ktIt->second;
      std::shared_ptr<Texture<double>>   u  = uIt->second;
      std::shared_ptr<Texture<double>>   v  = vIt->second;

      material[id] = makeGlassMaterial(kd, kt, u, v, ppt);
    } else if (mt == RavenMaterialType::RPlastic) {
      std::string kdId    = ppt.pointerList[0].id;
      std::string ksId    = ppt.pointerList[1].id;
      std::string roughId = ppt.pointerList[2].id;

      auto kdIt    = spectrumTexture.find(kdId);
      auto ksIt    = spectrumTexture.find(ksId);
      auto roughIt = floatTexture.find(roughId);

      std::shared_ptr<Texture<Spectrum>> kd    = kdIt->second;
      std::shared_ptr<Texture<Spectrum>> ks    = ksIt->second;
      std::shared_ptr<Texture<double>>   rough = roughIt->second;

      material[id] = makePlasticMaterial(kd, ks, rough, ppt);
    }
  }

  // generate shape
  for (size_t i = 0; i < shapeProperty.size(); i++) {
    auto [id, st, ppt] = shapeProperty[i];
    if (st == RavenShapeType::RMesh) {
      std::string LTWId = ppt.pointerList[0].id;
      std::string WTLId = ppt.pointerList[1].id;

      auto LTWIt = transform.find(LTWId);
      auto WTLIt = transform.find(WTLId);

      std::shared_ptr<TriangleMesh> triangleMesh =
          makeTriangleMesh(LTWIt->second, WTLIt->second, ppt);

      meshes.push_back(triangleMesh);
      std::vector<std::shared_ptr<Triangle>> tris =
          triangleMesh->getTriangles();
      meshList[id] = tris;
    } else if (st == RavenShapeType::RSphere) {
      std::string LTWId = ppt.pointerList[0].id;
      std::string WTLId = ppt.pointerList[1].id;

      auto LTWIt = transform.find(LTWId);
      auto WTLIt = transform.find(WTLId);

      shape[id] = makeSphereShape(LTWIt->second, WTLIt->second, ppt);
    }
  }

  // generate light
  for (size_t i = 0; i < lightProperty.size(); i++) {
    auto [id, st, ppt] = lightProperty[i];
    if (st == RavenLightType::RDiffuseArea) {
      std::string shapeId = ppt.pointerList[0].id;
      auto        shapeIt = shape.find(shapeId);

      auto                       transformIt = transform.find("identity");
      std::shared_ptr<Transform> I           = transformIt->second;
      if (shapeIt != shape.end()) {
        find shape std::shared_ptr<Shape> s = shapeIt->second;
        std::shared_ptr<Light> l = makeDiffuseAreaLight(I, I, s, ppt);
        light[id]                = l;
        sceneLights.push_back(l);
      } else {
        auto shapeListIt                            = meshList.find(shapeId);
        std::vector<std::shared_ptr<Triangle>> tris = shapeListIt->second;
        std::vector<std::shared_ptr<Light>>    lights;
        for (size_t j = 0; j < tris.size(); j++) {
          std::shared_ptr<Light> lighti =
              makeDiffuseAreaLight(I, I, tris[j], ppt);
          lights.push_back(lighti);
          sceneLights.push_back(lighti);
        }
        lightList[id] = lights;
      }
    }
  }

  // gnenerate primitive
  for (size_t i = 0; i < primProperty.size(); i++) {
    auto [id, pt, ppt] = primProperty[i];
    if (pt == RavenPrimitiveType::RPrimitive) {
      std::string shapeId = ppt.pointerList[0].id;
      std::string mateId  = ppt.pointerList[1].id;

      auto shapeIt = shape.find(shapeId);
      auto mateIt  = material.find(mateId);
      if (shapeIt != shape.end()) {
        std::shared_ptr<Shape>    s    = shapeIt->second;
        std::shared_ptr<Material> mate = mateIt->second;
        if (ppt.pointerList.size() == 3) {
          std::string                lightId = ppt.pointerList[2].id;
          auto                       lightIt = light.find(lightId);
          std::shared_ptr<Primitive> p =
              makePrimitive(s, mate, lightIt->second, ppt);
          primitiveList.push_back(p);
          primitive[id] = p;
        } else {
          std::shared_ptr<Primitive> p = makePrimitive(s, mate, nullptr, ppt);
          primitiveList.push_back(p);
          primitive[id] = p;
        }
      } else {
        auto shapeListIt = meshList.find(shapeId);
        auto mateIt      = material.find(mateId);

        std::vector<std::shared_ptr<Triangle>> tris = shapeListIt->second;
        std::shared_ptr<Material>              mate = mateIt->second;
        if (ppt.pointerList.size() == 3) {
          std::string lightId     = ppt.pointerList[2].id;
          auto        lightListIt = lightList.find(lightId);
          std::vector<std::shared_ptr<Primitive>> ps;
          std::vector<std::shared_ptr<Light>>     ls = lightListIt->second;
          for (size_t j = 0; j < tris.size(); j++) {
            std::shared_ptr<Primitive> p =
                makePrimitive(tris[j], mate, ls[j], ppt);
            primitiveList.push_back(p);
            ps.push_back(p);
          }
          plm[id] = ps;
        } else {
          std::vector<std::shared_ptr<Primitive>> ps;
          for (size_t j = 0; j < tris.size(); j++) {
            std::shared_ptr<Primitive> p =
                makePrimitive(tris[j], mate, nullptr, ppt);
            primitiveList.push_back(p);
            ps.push_back(p);
          }
          plm[id] = ps;
        }
      }
    }
  }

  // generate rendering pipeline
  std::shared_ptr<Scene> scene = std::make_shared<Scene>(
      ts, sceneLights, meshes, primitiveList, accelerate);
  std::shared_ptr<RavenRenderingPrograme> programe =
      std::make_shared<RavenRenderingPrograme>(renderer, scene, ts);
  return programe;
}
}  // namespace Raven