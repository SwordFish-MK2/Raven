#ifndef _RAVEN_UTILS_PARSER_H_
#define _RAVEN_UTILS_PARSER_H_

#include <Raven/core/accelerate.h>
#include <Raven/core/base.h>
#include <Raven/core/math.h>
#include <Raven/core/spectrum.h>
#include <Raven/utils/propertylist.h>
#include <tinyxml2.h>

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "Raven/core/integrator.h"
#include "Raven/core/primitive.h"

namespace Raven {
enum RavenFilmType {
  NoFilter
};

enum RavenCameraType {
  RPerspective,
  ROrthographic
};
enum RavenMaterialType {
  RMatte,
  RGlass,
  RMirror,
  RPlastic
};
enum RavenShapeType {
  RMesh,
  RSphere
};
enum RavenTextureType {
  RConstSpectrum,
  RConstFloat,
  RCheckerboardSpectrum,
  RCheckerboardFloat,
  RImageMapSpectrum,
  RImageMapFloat
};
enum RavenRendererType {
  RPath
};
enum RavenLightType {
  RDiffuseArea
};
enum RavenPrimitiveType {
  RPrimitive,
  RTransformed
};
enum RavenTransformType {
  RTranslate,
  RRotate,
  RRotateX,
  RRotateY,
  RRotateZ,
  RScale,
  RLookat,
  RIdentity,
  RInverse
};
enum RavenMappingType {
  RUVMapping,
  RSphereMapping
};

class FilmGenerator {
 private:
  std::vector<RavenClass> my_class;
};

class Parser {
 public:
  bool parse(const std::string& filePath);
  // std::shared_ptr<RavenRenderingPrograme> makePrograme()const;
 private:
  AccelType accelerate;

  std::shared_ptr<Camera>                 cam;
  std::shared_ptr<Integrator>             renderer;
  std::unique_ptr<Integrator>             integrator;
  std::vector<std::shared_ptr<Primitive>> prims;
  void                                    postProcess();

  std::pair<RavenCameraType, PropertyList>   cameraProperty;
  std::pair<RavenRendererType, PropertyList> integratorProperty;
  std::pair<RavenFilmType, PropertyList>     filmProperty;
  
  std::vector<std::tuple<std::string, RavenTransformType, PropertyList>>
      transformProperty;
  std::vector<std::tuple<std::string, RavenTextureType, PropertyList>>
      textureProperty;
  std::vector<std::tuple<std::string, RavenMaterialType, PropertyList>>
      materialProperty;
  std::vector<std::tuple<std::string, RavenShapeType, PropertyList>>
      shapeProperty;
  std::vector<std::tuple<std::string, RavenLightType, PropertyList>>
      lightProperty;
  std::vector<std::tuple<std::string, RavenPrimitiveType, PropertyList>>
      primProperty;
  std::vector<std::tuple<std::string, RavenMappingType, PropertyList>>
      mappingProperty;
};

}  // namespace Raven
#endif