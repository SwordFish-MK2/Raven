#include<Raven/core/base.h>

namespace Raven {
	std::vector<std::string> tokenize(
		const std::string& string,
		const std::string& delim,
		bool includeEmpty) {
		std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
		std::vector<std::string> tokens;

		while (lastPos != std::string::npos) {
			if (pos != lastPos || includeEmpty)
				tokens.push_back(string.substr(lastPos, pos - lastPos));
			lastPos = pos;
			if (lastPos != std::string::npos) {
				lastPos += 1;
				pos = string.find_first_of(delim, lastPos);
			}
		}

		return tokens;
	}
}