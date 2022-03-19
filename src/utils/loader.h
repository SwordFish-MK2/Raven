#ifndef _RAVEN_UTILS_LOADER_H_
#define _RAVEN_UTILS_LOADER_H_

#include<string_view>
#include"../core/base.h"
#include"../shape/mesh.h"
#include<optional>

namespace Raven {

	class Loader {
	public:
		std::optional<TriangleInfo> loadObj(const std::string_view& path);
	};
}
#endif