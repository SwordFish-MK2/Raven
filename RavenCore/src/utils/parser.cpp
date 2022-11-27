
#include<iostream>
#include<functional>

#include<Raven/utils/parser.h>
#include<Raven/core/film.h>
#include<Raven/core/transform.h>
#include<Raven/camera/projectiveCamera.h>
#include<Raven/core/renderer.h>
#include<Raven/textute/solidTexture.h>
#include<Raven/textute/imageTexture.h>
#include<Raven/textute/mapping.h>
#include<Raven/material/matte.h>
#include<Raven/material/glass.h>
#include<Raven/material/matte.h>
#include<Raven/material/plastic.h>
#include<Raven/shape/mesh.h>
#include<Raven/shape/sphere.h>
#include<Raven/core/shape.h>
#include<Raven/light/areaLight.h>
#include<Raven/utils/loader.h>
#include<Raven/material/mirror.h>

#define ParserErorr(x) std::cerr<< x <<std::endl

namespace Raven {
	bool Parser::parse(const std::string& file) {
		enum RTag {
			//classes
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

			//proprerties
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
			RNormal,
			RPointer
		};

		//create mapping between xml data to raven tag
		std::map<std::string, RTag> mapping;
		mapping["scene"] = RTag::RScene;
		mapping["film"] = RTag::RFilm;
		mapping["camera"] = RTag::RCamera;
		mapping["integrator"] = RTag::RIntegrator;
		mapping["texture"] = RTag::RTexture;
		mapping["material"] = RTag::RMaterial;
		mapping["shape"] = RTag::RShape;
		mapping["emit"] = RTag::REmit;
		mapping["primitive"] = RTag::RPrimitive;
		mapping["mapping"] = RTag::RMapping;
		mapping["transform"] = RTag::RTransform;
		mapping["boolean"] = RTag::RBoolean;
		mapping["integer"] = RTag::RInteger;
		mapping["float"] = RTag::RFloat;
		mapping["string"] = RTag::RString;
		mapping["vector2"] = RTag::RVector2;
		mapping["point2"] = RTag::RPoint2;
		mapping["vector3"] = RTag::RVector3;
		mapping["point3"] = RTag::RPoint3;
		mapping["spectrum"] = RTag::RSpectra;
		mapping["normal"] = RTag::RNormal;
		mapping["ref"] = RTag::RPointer;

		//a helper function that parses xml 
		std::function<void(tinyxml2::XMLElement*, PropertyList*)> parseTag =
			[&](tinyxml2::XMLElement* node, PropertyList* pList) {
			auto it = mapping.find(node->Name());
			int tag = it->second;

			//TODO::no matching mapping is found

			bool isScene = tag == RTag::RScene;
			bool isClass = tag < RTag::RPropertyStart&& tag != RTag::RScene;
			if (isScene) {
				std::string accType = node->FindAttribute("accelerate")->Value();
				if (accType == "bvh") {
					accelerate = AccelType::BVH;
				}
				else if (accType == "kdtree") {
					accelerate = AccelType::KdTree;
				}
				else {
					accelerate = AccelType::List;
				}

				//if current node is raven scene,loop over all child node to set primitive
				tinyxml2::XMLElement* childNode = node->FirstChildElement();
				while (childNode) {
					parseTag(childNode, nullptr);
					childNode = childNode->NextSiblingElement();
				}
			}
			if (isClass) {
				PropertyList list;
				//if current node is raven class, create a propertylist and loop over all child nodes to get its property
				tinyxml2::XMLElement* childNode = node->FirstChildElement();
				while (childNode) {
					parseTag(childNode, &list);
					childNode = childNode->NextSiblingElement();
				}

				//bind propertylist to related raven class
				switch (tag) {
				case RFilm: {
					std::map<std::string, RavenFilmType> filmMap;
					filmMap["nofilter"] = RavenFilmType::NoFilter;

					std::string type = node->FindAttribute("type")->Value();
					auto filmIt = filmMap.find(type);
					RavenFilmType filmTag = filmIt->second;

					std::pair<RavenFilmType, PropertyList> fProperty = std::make_pair(filmTag, list);
					filmProperty = fProperty;
					break;
				}
				case RCamera: {
					std::map<std::string, RavenCameraType> cameraMap;
					cameraMap["perspective"] = RavenCameraType::RPerspective;
					cameraMap["orthographic"] = RavenCameraType::ROrthographic;

					std::string type = node->FindAttribute("type")->Value();
					auto cameraIt = cameraMap.find(type);

					std::pair<RavenCameraType, PropertyList> cProperty = std::make_pair(cameraIt->second, list);
					cameraProperty = cProperty;
					break; }
				case RIntegrator: {
					std::map<std::string, RavenRendererType> integratorMap;
					integratorMap["path"] = RavenRendererType::RPath;

					std::string type = node->FindAttribute("type")->Value();
					auto integratorIt = integratorMap.find(type);

					std::pair<RavenRendererType, PropertyList> iProperty = std::make_pair(integratorIt->second, list);
					integratorProperty = iProperty;
					break; }
				case RTexture: {
					std::map<std::string, RavenTextureType> texMap;
					texMap["constspec"] = RavenTextureType::RConstSpectrum;
					texMap["constfloat"] = RavenTextureType::RConstFloat;
					texMap["checkspec"] = RavenTextureType::RCheckerboardSpectrum;
					texMap["checkfloat"] = RavenTextureType::RCheckerboardFloat;
					texMap["mapfloat"] = RavenTextureType::RImageMapFloat;
					texMap["mapspec"] = RavenTextureType::RImageMapSpectrum;

					std::string type = node->FindAttribute("type")->Value();
					std::string id = node->FindAttribute("id")->Value();
					auto texIt = texMap.find(type);

					std::tuple<std::string, RavenTextureType, PropertyList> tProperty =
						std::make_tuple(id, texIt->second, list);
					textureProperty.push_back(tProperty);
					break; }
				case RMaterial: {
					std::map<std::string, RavenMaterialType> mateMap;
					mateMap["matte"] = RavenMaterialType::RMatte;
					mateMap["glass"] = RavenMaterialType::RGlass;
					mateMap["mirror"] = RavenMaterialType::RMirror;
					mateMap["plastic"] = RavenMaterialType::RPlastic;

					std::string type = node->FindAttribute("type")->Value();
					std::string id = node->FindAttribute("id")->Value();
					auto mateIt = mateMap.find(type);

					std::tuple<std::string, RavenMaterialType, PropertyList> mProperty =
						std::make_tuple(id, mateIt->second, list);
					materialProperty.push_back(mProperty);
					break; }
				case RShape: {
					std::map<std::string, RavenShapeType> shapeMap;
					shapeMap["sphere"] = RavenShapeType::RSphere;
					shapeMap["mesh"] = RavenShapeType::RMesh;

					std::string type = node->FindAttribute("type")->Value();
					std::string id = node->FindAttribute("id")->Value();
					auto shapeIt = shapeMap.find(type);

					std::tuple<std::string, RavenShapeType, PropertyList> sProperty =
						std::make_tuple(id, shapeIt->second, list);
					shapeProperty.push_back(sProperty);
					break; }
				case REmit: {
					std::map<std::string, RavenLightType> lightMap;
					lightMap["diffuseArea"] = RavenLightType::RDiffuseArea;

					std::string type = node->FindAttribute("type")->Value();
					std::string id = node->FindAttribute("id")->Value();
					auto lightIt = lightMap.find(type);

					std::tuple<std::string, RavenLightType, PropertyList> lProperty =
						std::make_tuple(id, lightIt->second, list);
					lightProperty.push_back(lProperty);
					break; }
				case RPrimitive: {
					std::map<std::string, RavenPrimitiveType> priMap;
					priMap["primitive"] = RavenPrimitiveType::RPrimitive;
					priMap["transprimitive"] = RavenPrimitiveType::RTransformed;

					std::string type = node->FindAttribute("type")->Value();
					std::string id = node->FindAttribute("id")->Value();
					auto priIt = priMap.find(type);

					std::tuple<std::string, RavenPrimitiveType, PropertyList> pProperty =
						std::make_tuple(id, priIt->second, list);
					primProperty.push_back(pProperty);
					break; }
				case RTransform: {
					std::map<std::string, RavenTransformType> tranMap;
					tranMap["identity"] = RavenTransformType::RIdentity;
					tranMap["lookat"] = RavenTransformType::RLookat;
					tranMap["translate"] = RavenTransformType::RTranslate;
					tranMap["rotate"] = RavenTransformType::RRotate;
					tranMap["rotatex"] = RavenTransformType::RRotateX;
					tranMap["rotatey"] = RavenTransformType::RRotateY;
					tranMap["rotatez"] = RavenTransformType::RRotateZ;
					tranMap["inverse"] = RavenTransformType::RInverse;
					tranMap["scale"] = RavenTransformType::RScale;

					std::string type = node->FindAttribute("type")->Value();
					std::string id = node->FindAttribute("id")->Value();
					auto tranIt = tranMap.find(type);

					std::tuple<std::string, RavenTransformType, PropertyList> tProperty =
						std::make_tuple(id, tranIt->second, list);
					transformProperty.push_back(tProperty);
					break; }
				case RMapping: {
					std::map<std::string, RavenMappingType> mappingMap;
					mappingMap["uv"] = RavenMappingType::RUVMapping;
					mappingMap["spherical"] = RavenMappingType::RSphereMapping;

					std::string type = node->FindAttribute("type")->Value();
					std::string id = node->FindAttribute("id")->Value();
					auto mappingIt = mappingMap.find(type);

					std::tuple < std::string, RavenMappingType, PropertyList> mProperty =
						std::make_tuple(id, mappingIt->second, list);
					mappingProperty.push_back(mProperty);
					break; }
				}
			}

			else {
				//current node is raven property, insert its value to property list
				switch (tag) {
				case RBoolean: {
					std::string name = node->FindAttribute("name")->Value();
					bool value = node->FindAttribute("value")->BoolValue();
					pList->setBoolean(name, value);
					break;
				}
				case RInteger: {
					std::string name = node->FindAttribute("name")->Value();
					int value = node->FindAttribute("value")->IntValue();
					pList->setInteger(name, value);
					break;
				}
				case RFloat: {
					std::string name = node->FindAttribute("name")->Value();
					double value = node->FindAttribute("value")->DoubleValue();
					pList->setFloat(name, value);
					break;
				}
				case RString: {
					std::string name = node->FindAttribute("name")->Value();
					std::string value = node->FindAttribute("value")->Value();
					pList->setString(name, value);
					break;
				}
				case RVector2: {
					std::string name = node->FindAttribute("name")->Value();
					const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
					Vector2f value = toVector2f(v->Value());
					pList->setVector2f(name, value);
					break;
				}
				case RVector3: {
					std::string name = node->FindAttribute("name")->Value();
					const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
					Vector3f value = toVector3f(v->Value());
					pList->setVector3f(name, value);
					break;
				}
				case RPoint3: {
					std::string name = node->FindAttribute("name")->Value();
					const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
					Point3f value = toPoint3f(v->Value());
					pList->setPoint3f(name, value);
					break;
				}
				case RPoint2: {
					std::string name = node->FindAttribute("name")->Value();
					const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
					Point2f value = toPoint2f(v->Value());
					pList->setPoint2f(name, value);
					break;
				}
				case RNormal: {
					std::string name = node->FindAttribute("name")->Value();
					const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
					Normal3f value = toNormal(v->Value());
					pList->setNormal3f(name, value);
					break;
				}
				case RSpectra: {
					std::string name = node->FindAttribute("name")->Value();
					std::string type = node->FindAttribute("type")->Value();
					const tinyxml2::XMLAttribute* v = node->FindAttribute("value");
					if (type == "rgb") {
						Vector3f rgbVec = toVector3f(v->Value());
						Spectrum rgbSpectra = Spectrum::fromRGB(rgbVec);
						pList->setSpectra(name, rgbSpectra);
					}
					else {
						//std::string vString = v->Value();
						//std::shared_ptr<double[]> lambda;
						//std::shared_ptr<double[]>spectrumValue;
						//int n;
						//getSpectrumSample(lambda, spectrumValue, &n, vString);
						//Spectrum sampledSpectra = Spectrum::fromSampled(&lambda[0], &spectrumValue[0], n);
						//pList->setSpectra(name, sampledSpectra);
					}
					break;
				}
				case RPointer: {
					std::map<std::string, RavenPointerType> pointerMap;
					pointerMap["transform"] = RavenPointerType::transform_pointer;
					pointerMap["mapping"] = RavenPointerType::mapping_pointer;
					pointerMap["shape"] = RavenPointerType::shape_pointer;
					pointerMap["texturefloat"] = RavenPointerType::texture_float_pointer;
					pointerMap["texturespectrum"] = RavenPointerType::texture_spectrum_pointer;
					pointerMap["emit"] = RavenPointerType::emit_pointer;
					pointerMap["material"] = RavenPointerType::material_pointer;
					pointerMap["primitive"] = RavenPointerType::primitive_pointer;

					std::string type = node->FindAttribute("type")->Value();
					std::string id = node->FindAttribute("id")->Value();
					auto pIt = pointerMap.find(type);

					//pList->setPointer(id, pIt->second);
					pList->pointerList.push_back(Pointer(id, pIt->second));
					break;
				}
				}
			}
		};

		const char* fileName = file.c_str();
		tinyxml2::XMLDocument doc;
		doc.LoadFile(fileName);

		tinyxml2::XMLElement* root = doc.RootElement();
		if (root) {
			//std::cout << root->Name() << std::endl;
			tinyxml2::XMLElement* child = root->FirstChildElement();
			while (child) {
				parseTag(child, nullptr);
				child = child->NextSiblingElement();

			}
		}
		return true;
	}

	std::shared_ptr<RavenRenderingPrograme> Parser::makePrograme()const {
		std::vector<std::shared_ptr<Transform>> ts;
		std::vector<std::shared_ptr<TriangleMesh>> meshes;
		std::vector<std::shared_ptr<Primitive>> primitiveList;
		std::vector<std::shared_ptr<Light>> sceneLights;
		std::map<std::string, std::shared_ptr<Transform>> transform;
		std::map<std::string, std::shared_ptr<Texture<Spectrum>>>spectrumTexture;
		std::map<std::string, std::shared_ptr<Texture<double>>>floatTexture;
		std::map<std::string, std::shared_ptr<Material>>material;
		std::map<std::string, std::shared_ptr<Shape>>shape;
		std::map<std::string, std::shared_ptr<Light>>light;
		std::map<std::string, std::shared_ptr<Primitive>>primitive;
		std::map<std::string, std::shared_ptr<TextureMapping2D>>mapping;
		std::map<std::string, std::vector<std::shared_ptr<Triangle>>> meshList;
		std::map<std::string, std::vector<std::shared_ptr<Light>>> lightList;
		std::map<std::string, std::vector<std::shared_ptr<Primitive>>>plm;

		//generate film
		auto ft = filmProperty.first;
		std::shared_ptr<Film> film;
		if (ft == RavenFilmType::NoFilter) {
			PropertyList ppt = filmProperty.second;
			film = makeFilm(ppt);
		}

		//generate camera
		auto ct = cameraProperty.first;
		std::shared_ptr<Camera> camera;
		if (ct == RavenCameraType::RPerspective) {
			//generate camera world
			PropertyList ppt = cameraProperty.second;
			Point3f pos = ppt.getPoint3f("pos");
			Point3f lookat = ppt.getPoint3f("lookat");
			Vector3f up = ppt.getVector3f("up");
			Transform cameraWorld = LookAt(pos, lookat, up);
			Transform raster = Raster(film->yRes, film->xRes);
			camera = makePerspectiveCamera(cameraWorld, raster, ppt);
		}
		else if (ct == RavenCameraType::ROrthographic) {
			PropertyList ppt = cameraProperty.second;
			Point3f pos = ppt.getPoint3f("pos");
			Point3f lookat = ppt.getPoint3f("lookat");
			Vector3f up = ppt.getVector3f("up");
			Transform cameraWorld = LookAt(pos, lookat, up);
			Transform raster = Raster(film->yRes, film->xRes);
			camera = makeOrthographicCamera(cameraWorld, raster, ppt);
		}

		//generate integrator
		auto rt = integratorProperty.first;
		std::shared_ptr<Renderer> renderer;
		if (rt == RavenRendererType::RPath) {
			PropertyList ppt = integratorProperty.second;
			renderer = makePathTracingRenderer(film, camera, ppt);
		}

		//generate scene
		// 
		//generate transform
		for (size_t i = 0; i < transformProperty.size(); i++) {

			auto [id, tt, ppt] = transformProperty[i];
			if (tt == RavenTransformType::RIdentity) {
				std::shared_ptr<Transform> trans = std::make_shared<Transform>(Identity());
				if (ppt.pointerList.size() != 0) {
					for (size_t j = 0; j < ppt.pointerList.size(); j++) {
						std::string tId = ppt.pointerList[j].id;
						auto tIt = transform.find(tId);
						std::shared_ptr<Transform> t = tIt->second;
						*trans = (*t) * (*trans);
					}
				}
				transform[id] = trans;
				ts.push_back(trans);
			}
			else if (tt == RavenTransformType::RRotate) {
				double angle = ppt.getFloat("angle");
				Vector3f axis = ppt.getVector3f("axis");
				std::shared_ptr<Transform> trans = std::make_shared<Transform>(Rotate(angle, axis));
				if (ppt.pointerList.size() != 0) {
					for (size_t j = 0; j < ppt.pointerList.size(); j++) {
						std::string tId = ppt.pointerList[j].id;
						auto tIt = transform.find(tId);
						std::shared_ptr<Transform> t = tIt->second;
						*trans = (*t) * (*trans);
					}
				}
				transform[id] = trans;
				ts.push_back(trans);
			}
			else if (tt == RavenTransformType::RRotateX) {
				double angle = ppt.getFloat("angle");
				std::shared_ptr<Transform> trans = std::make_shared<Transform>(RotateX(angle));
				if (ppt.pointerList.size() != 0) {
					for (size_t j = 0; j < ppt.pointerList.size(); j++) {
						std::string tId = ppt.pointerList[j].id;
						auto tIt = transform.find(tId);
						std::shared_ptr<Transform> t = tIt->second;
						*trans = (*t) * (*trans);
					}
				}
				transform[id] = trans;
				ts.push_back(trans);
			}
			else if (tt == RavenTransformType::RRotateY) {
				double angle = ppt.getFloat("angle");
				std::shared_ptr<Transform>trans = std::make_shared<Transform>(RotateY(angle));
				if (ppt.pointerList.size() != 0) {
					for (size_t j = 0; j < ppt.pointerList.size(); j++) {
						std::string tId = ppt.pointerList[j].id;
						auto tIt = transform.find(tId);
						std::shared_ptr<Transform> t = tIt->second;
						*trans = (*t) * (*trans);
					}
				}
				transform[id] = trans;
				ts.push_back(trans);
			}
			else if (tt == RavenTransformType::RRotateZ) {
				double angle = ppt.getFloat("angle");
				std::shared_ptr<Transform>trans = std::make_shared<Transform>(RotateZ(angle));
				if (ppt.pointerList.size() != 0) {
					for (size_t j = 0; j < ppt.pointerList.size(); j++) {
						std::string tId = ppt.pointerList[j].id;
						auto tIt = transform.find(tId);
						std::shared_ptr<Transform> t = tIt->second;
						*trans = (*t) * (*trans);
					}
				}
				transform[id] = trans;
				ts.push_back(trans);
			}
			else if (tt == RavenTransformType::RScale) {
				Vector3f vector = ppt.getVector3f("scale");
				std::shared_ptr<Transform>trans = std::make_shared<Transform>(Scale(vector));
				if (ppt.pointerList.size() != 0) {
					for (size_t j = 0; j < ppt.pointerList.size(); j++) {
						std::string tId = ppt.pointerList[j].id;
						auto tIt = transform.find(tId);
						std::shared_ptr<Transform> t = tIt->second;
						*trans = (*t) * (*trans);
					}
				}
				transform[id] = trans;
				ts.push_back(trans);
			}
			else if (tt == RavenTransformType::RTranslate) {
				Vector3f vector = ppt.getVector3f("translate");
				std::shared_ptr<Transform>trans = std::make_shared<Transform>(Translate(vector));
				if (ppt.pointerList.size() != 0) {
					for (size_t j = 0; j < ppt.pointerList.size(); j++) {
						std::string tId = ppt.pointerList[j].id;
						auto tIt = transform.find(tId);
						std::shared_ptr<Transform> t = tIt->second;
						*trans = (*t) * (*trans);
					}
				}
				transform[id] = trans;
				ts.push_back(trans);
			}
			else if (tt == RavenTransformType::RInverse) {
				std::string tId = ppt.pointerList[0].id;
				auto tIt = transform.find(tId);
				std::shared_ptr<Transform> t = tIt->second;
				std::shared_ptr<Transform> trans = std::make_shared<Transform>(t->inverse());
				transform[id] = trans;
				ts.push_back(trans);
			}
		}

		//generate mapping
		for (size_t i = 0; i < mappingProperty.size(); i++) {
			auto [id, mt, ppt] = mappingProperty[i];
			if (mt == RavenMappingType::RUVMapping) {
				mapping[id] = makeUVMapping(ppt);
			}
			else if (mt == RavenMappingType::RSphereMapping) {
				std::string transformId = ppt.pointerList[0].id;
				auto transformIt = transform.find(transformId);

				mapping[id] = makeSphericalMapping2D(transformIt->second, ppt);
			}
		}

		//generate texture
		for (size_t i = 0; i < textureProperty.size(); i++) {
			auto [id, tt, ppt] = textureProperty[i];
			if (tt == RavenTextureType::RConstFloat) {
				floatTexture[id] = makeConstTextureFloat(ppt);
			}
			else if (tt == RavenTextureType::RConstSpectrum) {
				spectrumTexture[id] = makeConstTextureSpectrum(ppt);
			}
			else if (tt == RavenTextureType::RCheckerboardFloat) {
				//get pointer
				std::string oddId = ppt.pointerList[0].id;
				std::string evenId = ppt.pointerList[1].id;
				std::string mappingId = ppt.pointerList[2].id;
				auto oddIt = floatTexture.find(oddId);
				auto evenIt = floatTexture.find(evenId);
				auto mappingIt = mapping.find(mappingId);

				floatTexture[id] = makeCheckeredFloat(oddIt->second, evenIt->second, mappingIt->second, ppt);
			}
			else if (tt == RavenTextureType::RCheckerboardSpectrum) {
				std::string oddId = ppt.pointerList[0].id;
				std::string evenId = ppt.pointerList[1].id;
				std::string mappingId = ppt.pointerList[2].id;
				auto oddIt = spectrumTexture.find(oddId);
				auto evenIt = spectrumTexture.find(evenId);
				auto mappingIt = mapping.find(mappingId);

				spectrumTexture[id] = makeCheckeredSpectrum(oddIt->second, evenIt->second, mappingIt->second, ppt);
			}
			else if (tt == RavenTextureType::RImageMapFloat) {
				std::string mappingId = ppt.pointerList[0].id;
				auto mappingIt = mapping.find(mappingId);
				floatTexture[id] = makeImageTextureFloat(mappingIt->second, ppt);
			}
			//else if (tt == RavenTextureType::RImageMapSpectrum) {
			//	std::string mappingId = ppt.pointerList[0].id;
			//	auto mappingIt = mapping.find(mappingId);
			//	spectrumTexture[id] = makeImageTextureSpectrum(mappingIt->second, ppt);
			//}
		}

		//generate material
		for (size_t i = 0; i < materialProperty.size(); i++) {
			auto [id, mt, ppt] = materialProperty[i];
			if (mt == RavenMaterialType::RMatte) {
				std::string spectrumId = ppt.pointerList[0].id;
				std::string floatId = ppt.pointerList[1].id;
				auto kdIt = spectrumTexture.find(spectrumId);
				auto sigmaIt = floatTexture.find(floatId);

				std::shared_ptr<Texture<Spectrum>>kd = kdIt->second;
				std::shared_ptr<Texture<double>>sigma = sigmaIt->second;

				material[id] = makeMatteMaterial(sigma, kd, ppt);
			}
			else if (mt == RavenMaterialType::RMirror) {
				std::string rId = ppt.pointerList[0].id;
				auto rIt = spectrumTexture.find(rId);

				std::shared_ptr<Texture<Spectrum>>r = rIt->second;
				material[id] = makeMirrorMaterial(r, ppt);
			}
			else if (mt == RavenMaterialType::RGlass) {
				std::string kdId = ppt.pointerList[0].id;
				std::string ktId = ppt.pointerList[1].id;
				std::string uId = ppt.pointerList[2].id;
				std::string vId = ppt.pointerList[3].id;

				auto kdIt = spectrumTexture.find(kdId);
				auto ktIt = spectrumTexture.find(ktId);
				auto uIt = floatTexture.find(uId);
				auto vIt = floatTexture.find(vId);

				std::shared_ptr<Texture<Spectrum>> kd = kdIt->second;
				std::shared_ptr<Texture<Spectrum>> kt = ktIt->second;
				std::shared_ptr<Texture<double>>u = uIt->second;
				std::shared_ptr<Texture<double>>v = vIt->second;

				material[id] = makeGlassMaterial(kd, kt, u, v, ppt);
			}
			else if (mt == RavenMaterialType::RPlastic) {
				std::string kdId = ppt.pointerList[0].id;
				std::string ksId = ppt.pointerList[1].id;
				std::string roughId = ppt.pointerList[2].id;

				auto kdIt = spectrumTexture.find(kdId);
				auto ksIt = spectrumTexture.find(ksId);
				auto roughIt = floatTexture.find(roughId);

				std::shared_ptr<Texture<Spectrum>> kd = kdIt->second;
				std::shared_ptr<Texture<Spectrum>> ks = ksIt->second;
				std::shared_ptr<Texture<double>> rough = roughIt->second;

				material[id] = makePlasticMaterial(kd, ks, rough, ppt);
			}
		}

		//generate shape
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
				std::vector<std::shared_ptr<Triangle>> tris = triangleMesh->getTriangles();
				meshList[id] = tris;
			}
			else if (st == RavenShapeType::RSphere) {
				std::string LTWId = ppt.pointerList[0].id;
				std::string WTLId = ppt.pointerList[1].id;

				auto LTWIt = transform.find(LTWId);
				auto WTLIt = transform.find(WTLId);

				shape[id] = makeSphereShape(LTWIt->second, WTLIt->second, ppt);
			}
		}

		//generate light
		for (size_t i = 0; i < lightProperty.size(); i++) {
			auto [id, st, ppt] = lightProperty[i];
			if (st == RavenLightType::RDiffuseArea) {
				std::string shapeId = ppt.pointerList[0].id;
				auto shapeIt = shape.find(shapeId);

				auto transformIt = transform.find("identity");
				std::shared_ptr<Transform> I = transformIt->second;
				if (shapeIt != shape.end()) {
					//find shape
					std::shared_ptr<Shape> s = shapeIt->second;
					std::shared_ptr<Light> l = makeDiffuseAreaLight(I, I, s, ppt);
					light[id] = l;
					sceneLights.push_back(l);
				}
				else {
					auto shapeListIt = meshList.find(shapeId);
					std::vector<std::shared_ptr<Triangle>> tris = shapeListIt->second;
					std::vector<std::shared_ptr<Light>> lights;
					for (size_t j = 0; j < tris.size(); j++) {
						std::shared_ptr<Light> lighti = makeDiffuseAreaLight(I, I, tris[j], ppt);
						lights.push_back(lighti);
						sceneLights.push_back(lighti);
					}
					lightList[id] = lights;
				}
			}
		}

		//gnenerate primitive
		for (size_t i = 0; i < primProperty.size(); i++) {
			auto [id, pt, ppt] = primProperty[i];
			if (pt == RavenPrimitiveType::RPrimitive) {
				std::string shapeId = ppt.pointerList[0].id;
				std::string mateId = ppt.pointerList[1].id;

				auto shapeIt = shape.find(shapeId);
				auto mateIt = material.find(mateId);
				if (shapeIt != shape.end()) {
					std::shared_ptr<Shape> s = shapeIt->second;
					std::shared_ptr<Material> mate = mateIt->second;
					if (ppt.pointerList.size() == 3) {
						std::string lightId = ppt.pointerList[2].id;
						auto lightIt = light.find(lightId);
						std::shared_ptr<Primitive> p = makePrimitive(s, mate, lightIt->second, ppt);
						primitiveList.push_back(p);
						primitive[id] = p;
					}
					else {
						std::shared_ptr<Primitive> p = makePrimitive(s, mate, nullptr, ppt);
						primitiveList.push_back(p);
						primitive[id] = p;
					}
				}
				else {
					auto shapeListIt = meshList.find(shapeId);
					auto mateIt = material.find(mateId);

					std::vector<std::shared_ptr<Triangle>> tris = shapeListIt->second;
					std::shared_ptr<Material> mate = mateIt->second;
					if (ppt.pointerList.size() == 3) {
						std::string lightId = ppt.pointerList[2].id;
						auto lightListIt = lightList.find(lightId);
						std::vector<std::shared_ptr<Primitive>> ps;
						std::vector<std::shared_ptr<Light>> ls = lightListIt->second;
						for (size_t j = 0; j < tris.size(); j++) {
							std::shared_ptr<Primitive> p = makePrimitive(tris[j], mate, ls[j], ppt);
							primitiveList.push_back(p);
							ps.push_back(p);
						}
						plm[id] = ps;
					}
					else {
						std::vector<std::shared_ptr<Primitive>> ps;
						for (size_t j = 0; j < tris.size(); j++) {
							std::shared_ptr<Primitive> p = makePrimitive(tris[j], mate, nullptr, ppt);
							primitiveList.push_back(p);
							ps.push_back(p);
						}
						plm[id] = ps;
					}
				}
			}

		}

		//generate rendering pipeline
		std::shared_ptr<Scene> scene = std::make_shared<Scene>(ts, sceneLights, meshes, primitiveList, accelerate);
		std::shared_ptr<RavenRenderingPrograme> programe = std::make_shared<RavenRenderingPrograme>(renderer, scene, ts);
		return programe;
	}
}