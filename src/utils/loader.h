#ifndef _RAVEN_UTILS_LOADER_H_
#define _RAVEN_UTILS_LOADER_H_

#include<string_view>
#include"../core/base.h"
#include"../shape/mesh.h"
#include<optional>
#include"../core/spectrum.h"


namespace Raven {

	class Loader {
	public:
		std::optional<TriangleInfo> loadObj(const std::string_view& path);

		std::optional<TriangleInfo> load(const std::string& fileName, const std::string& mtlBasePath);
	};


	Vector3f toVector(const unsigned char* data);

	Spectrum toSpectrum(const unsigned char* data);

	std::unique_ptr<Vector3f[]> ReadImageVector(const std::string& path, Point2i* resolution);

	std::unique_ptr<Spectrum[]>ReadImageSpectrum(const std::string& path, Point2i* resolution);
}
#endif