//#ifndef _RAVEN_UTILS_PARSER_H_
//#define _RAVEN_UTILS_PARSER_H_
//
//#include<optional>
//#include<string>
//#include<map>
//#include<memory>
//#include<tinyxml/tinyxml2.h>
//#include"../core/base.h"
//#include"../core/spectrum.h"
//#include"../core/math.h"
//#include"../core/programe.h"
//#include"propertylist.h"
//namespace Raven {
//	enum RavenFilmType {
//		NoFilter
//	};
//
//	enum RavenCameraType {
//		RPerspective,
//		ROrthographic
//	};
//	enum RavenMaterialType {
//		RMatte,
//		RGlass,
//		RMirror,
//		RPlastic
//	};
//	enum RavenShapeType {
//		RMesh,
//		RSphere
//	};
//	enum RavenTextureType {
//		RConstSpectrum,
//		RConstFloat,
//		RCheckerboardSpectrum,
//		RCheckerboardFloat,
//		RImageMapSpectrum,
//		RImageMapFloat
//	};
//	enum RavenRendererType {
//		RPath
//	};
//	enum RavenLightType {
//		RDiffuseArea
//	};
//	enum RavenPrimitiveType {
//		RPrimitive,
//		RTransformed
//	};
//	enum RavenTransformType {
//		RTranslate,
//		RRotate,
//		RRotateX,
//		RRotateY,
//		RRotateZ,
//		RScale,
//		RLookat,
//		RIdentity,
//		RInverse
//	};
//	enum RavenMappingType {
//		RUVMapping,
//		RSphereMapping
//	};
//
//	class Parser {
//	public:
//		bool parse(const std::string& filePath);
//		std::shared_ptr<RavenRenderingPrograme> makePrograme()const;
//	private:
//		AccelType accelerate;
//
//		std::shared_ptr<Camera> cam;
//		std::shared_ptr<Renderer> renderer;
//
//		std::pair<RavenCameraType, PropertyList> cameraProperty;
//		std::pair<RavenRendererType, PropertyList> integratorProperty;
//		std::pair<RavenFilmType, PropertyList> filmProperty;
//
//		std::vector<std::tuple<std::string, RavenTransformType, PropertyList>>transformProperty;
//		std::vector<std::tuple<std::string, RavenTextureType, PropertyList>>textureProperty;
//		std::vector<std::tuple<std::string, RavenMaterialType, PropertyList>>materialProperty;
//		std::vector<std::tuple<std::string, RavenShapeType, PropertyList>>shapeProperty;
//		std::vector<std::tuple<std::string, RavenLightType, PropertyList>>lightProperty;
//		std::vector<std::tuple<std::string, RavenPrimitiveType, PropertyList>>primProperty;
//		std::vector<std::tuple<std::string, RavenMappingType, PropertyList>>mappingProperty;
//
//	};
//}
//#endif