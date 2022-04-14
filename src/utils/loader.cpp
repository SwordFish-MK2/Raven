#define STB_IMAGE_IMPLEMENTATION

#include"loader.h"
#include<filesystem>
#include<fstream>
namespace Raven {
	std::optional<TriangleInfo> Loader::loadObj(const std::string_view& path) {
		if (path.empty())
			return std::nullopt;
		if (!std::filesystem::exists(path))
			return std::nullopt;

		std::vector<Point3f> vertices;
		std::vector<Normal3f> normals;
		std::vector<Point2f> uvs;
		std::vector<Vector3f> tangants;
		std::vector<int>indices;
		int number = 0;
		std::ifstream in(path.data(), std::ifstream::in);

		std::string line;
		while (std::getline(in, line))
		{
			if (line[0] == 'v')
			{
				if (line[1] == ' ')
				{
					std::istringstream iss(line.substr(2));
					Point3f p;
					iss >> p.x >> p.y >> p.z;
					vertices.push_back(p);
				}
				else if (line[1] == 'n' && line[2] == ' ')
				{
					std::istringstream iss(line.substr(2));
					Normal3f n;
					iss >> n.x >> n.y >> n.z;
					normals.push_back(n);
				}
				else if (line[1] == 't' && line[2] == ' ')
				{
					std::istringstream iss(line.substr(2));
					Point2f uv;
					iss >> uv.x >> uv.y;
					uvs.push_back(uv);
				}
			}
			else if (line[0] == 'f' && line[1] == ' ')
			{
				number++;
				int index[3];
				std::istringstream iss(line.substr(2));
				iss >> index[0] >> index[1] >> index[2];
				indices.push_back(index[0] - 1);
				indices.push_back(index[1] - 1);
				indices.push_back(index[2] - 1);
			}
		}
		in.close();

		if (normals.empty()) {
			normals.resize(vertices.size());
			for (int i = 0; i < indices.size(); i += 3) {
				const Point3f& p0 = vertices[indices[i]];
				const Point3f& p1 = vertices[indices[i + 1]];
				const Point3f& p2 = vertices[indices[i + 2]];
				Vector3f e1 = p1 - p0;
				Vector3f e2 = p2 - p1;
				Normal3f n = Normalize(Normal3f(Cross(e1, e2)));
				normals[indices[i]] = n;
				normals[indices[i + 1]] = n;
				normals[indices[i + 2]] = n;
			}
		}
		TriangleInfo info(number, vertices, normals, uvs, tangants, indices);
		return std::optional<TriangleInfo>(info);
	}

	Vector3f toVector(const unsigned char* data) {
		return Vector3f(data[0] / 256.0, data[1] / 256.0, data[2] / 256.0);
	}

	Spectrum toSpectrum(const unsigned char* data) {
		double value[3] = { double(data[0]) / 256.0,double(data[1]) / 256.0,double(data[2]) / 256.0 };
		return RGBSpectrum::fromRGB(&value[0]);
	}

	std::unique_ptr<Vector3f[]> ReadImageVector(const std::string& path, Point2i* resolution) {
		const char* filename = (char*)path.c_str();
		int w, h, c;
		unsigned char* data = stbi_load(filename, &w, &h, &c, 0);
		*resolution = Point2i(w, h);
		std::unique_ptr<Vector3f[]> convertedData = std::make_unique<Vector3f[]>(w * h);
		//TODO::未读取到图片
		//TODO::判断图片的通道数
		//如果图片为三通道
		for (int i = 0; i < w * h; i++) {
			convertedData[i] = toVector(&data[i * 3]);

		}
		stbi_image_free(data);
		return convertedData;
	}

	std::unique_ptr<Spectrum[]>ReadImageSpectrum(const std::string& path, Point2i* resolution) {
		const char* filename = (char*)path.c_str();
		int w, h, c;
		unsigned char* data = stbi_load(filename, &w, &h, &c, 0);
		*resolution = Point2i(w, h);
		std::unique_ptr<Spectrum[]> convertedData = std::make_unique<Spectrum[]>(w * h);
		for (int i = 0; i < w * h; i++)
			convertedData[i] = toSpectrum(&data[i * 3]);
		return convertedData;
	}
}