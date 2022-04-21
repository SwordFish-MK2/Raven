#ifndef _RAVEN_UTILS_LOADER_H_
#define _RAVEN_UTILS_LOADER_H_

#include<string_view>
#include"../core/base.h"
#include"../shape/mesh.h"
#include<optional>
#include"../core/spectrum.h"


namespace Raven {
	//class SceneReader {
	//public:
	//	void parseXML(const std::string_view& xmlPath);
	//	static std::map<std::string, std::string> dataMap;
	//};

	class Loader {
	public:
		std::optional<TriangleInfo> load(
			const std::string& path, 
			const std::string& fileName, 
			const std::string& mtlBasePath = std::string(""));
	};

	Vector3f toVector(const unsigned char* data);

	Spectrum toSpectrum(const unsigned char* data);

	std::unique_ptr<Vector3f[]> ReadImageVector(const std::string& path, Point2i* resolution);

	std::unique_ptr<Spectrum[]>ReadImageSpectrum(const std::string& path, Point2i* resolution);
}
#endif