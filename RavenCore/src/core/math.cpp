#include<Raven/core/math.h>

namespace Raven {


	double toFloat(const std::string& str) {
		char* end_ptr = nullptr;
		double result = (double)strtof(str.c_str(), &end_ptr);
		if (*end_ptr != '\0' ) {
			std::cerr << "Failed to parse float value::" << str << std::endl;
		}
		return result;
	}

	double toInt(const std::string& str) {
		char* end_ptr = nullptr;
		int result = (int)strtof(str.c_str(), &end_ptr);
		if (*end_ptr != '\0') {
			std::cerr << "Fauled to parse int value" << str << std::endl;
		}
		return result;
	}

	Point3f toPoint3f(const std::string& str) {
		std::vector<std::string> tokens = tokenize(str);
		if (tokens.size() == 3) {
			Point3f result;
			for (int i = 0; i < 3; i++) {
				result[i] = toFloat(tokens[i]);
			}
			return result;
		}
		else {
			std::cerr << "Failed to parse point3f" << str << std::endl;
			return Point3f(0);
		}
	}

	Point2f toPoint2f(const std::string& str) {
		std::vector<std::string> tokens = tokenize(str);
		if (tokens.size() == 2) {
			Point2f result;
			for (int i = 0; i < 2; i++) {
				result[i] = toFloat(tokens[i]);
			}
			return result;
		}
		else {
			std::cerr << "Failed to parse point2f" << str << std::endl;
			return Point2f(0);
		}
	}

	Vector3f toVector3f(const std::string& str) {
		std::vector<std::string> tokens = tokenize(str);
		if (tokens.size() == 3) {
			Vector3f result;
			for (int i = 0; i < 3; i++) {
				result[i] = toFloat(tokens[i]);
			}
			return result;
		}
		else {
			std::cerr << "Failed to parse point3f" << str << std::endl;
			return Vector3f(0);
		}
	}

	Vector2f toVector2f(const std::string& str) {
		std::vector<std::string> tokens = tokenize(str);
		if (tokens.size() == 2) {
			Vector2f result;
			for (int i = 0; i < 2; i++) {
				result[i] = toFloat(tokens[i]);
			}
			return result;
		}
		else {
			std::cerr << "Failed to parse point3f" << str << std::endl;
			return Vector2f(0);
		}
	}

	Normal3f toNormal(const std::string& str) {
		std::vector<std::string> tokens = tokenize(str);
		if (tokens.size() == 3) {
			Normal3f result;
			for (int i = 0; i < 3; i++) {
				result[i] = toFloat(tokens[i]);
			}
			return result;
		}
		else {
			std::cerr << "Failed to parse point3f" << str << std::endl;
			return Normal3f(0);
		}
	}
}