#ifndef _RAVEN_UTILS_LOADER_H_
#define _RAVEN_UTILS_LOADER_H_

#include<string_view>
#include<Raven/core/base.h>
#include<Raven/shape/mesh.h>
#include<optional>
#include<Raven/core/spectrum.h>


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
			const std::string& mtlBasePath = std::string(""));

		Loader(const std::string& prj_path = RAVEN_PATH) :prj_path(prj_path) {}
	private:
		std::string prj_path;
	};

	Vector3f toVector(const unsigned char* data);



	std::unique_ptr<Vector3f[]> ReadImageVector(const std::string& path, Point2i* resolution);


}
#endif