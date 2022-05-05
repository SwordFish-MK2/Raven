#ifndef _RAVEN_UTILS_PARSER_H_
#define _RAVEN_UTILS_PARSER_H_


#include<string>
#include<map>
#include<memory>

#include"../core/texture.h"
#include"../core/base.h"
#include"../core/spectrum.h"

namespace Raven {
	void Parse() {
		tinyxml2::XMLDocument doc;
		doc.LoadFile("C:/Users/Desktop/scene.xml");
		tinyxml2::XMLElement* root = doc.RootElement();


	}

	class Parser {
	public:
		bool parse(const std::string& filePath);
	private:
		std::map<std::string, std::shared_ptr<Texture<double>>> floatTextureMap;
		std::map<std::string, std::shared_ptr<Texture<Spectrum>>> spectrumTextureMap;
		std::map<std::string, std::shared_ptr<Material>> mateMap;
		std::map<std::string, std::shared_ptr<Shape>> shapeMap;
		std::map<std::string, std::shared_ptr<Light>> lightMap;
		std::map<std::string, std::shared_ptr<Primitive>> priMap;
	};
}
#endif