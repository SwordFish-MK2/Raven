#include"parser.h"
#include<tinyxml/tinyxml2.h>
#include"../core/film.h"

namespace Raven {
	bool Parser::parse(const std::string& filePath) {
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError error = doc.LoadFile(filePath.c_str());
		if (error != 0) {
			std::cout << "Raven Failed to load file: " << filePath << std::endl;
			return false;
		}

		tinyxml2::XMLElement* root = doc.RootElement();

		//parse film
		tinyxml2::XMLElement* film = root->FirstChildElement("film");
		int filmU = film->FindAttribute("uSize")->IntValue();
		int filmV = film->FindAttribute("vSize")->IntValue();
		Film film(filmU, filmV);

		//parse camera
		tinyxml2::XMLElement* camera = root->FirstChildElement("camera");
		const tinyxml2::XMLAttribute* cType = camera->FindAttribute("type");


	}
}