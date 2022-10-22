

#define TINYOBJLOADER_IMPLEMENTATION
#include<tinyobjloader/tiny_obj_loader.h>

#include"loader.h"
#include<filesystem>
#include<fstream>
namespace Raven {

	std::optional<TriangleInfo> Loader::load(
		const std::string& path,
		const std::string& fileName,
		const std::string& mtlBasePath) {
		const std::string filePath = path + fileName;
		tinyobj::attrib_t attrib;//存数据
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warning;
		std::string error;

		const char* file_name = filePath.c_str();
		const char* mtl_base_path = mtlBasePath.c_str();

		bool ret =
			tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, file_name, mtl_base_path, true);

		if (!warning.empty())
			std::cout << "Obj loader warning: " << warning << std::endl;

		if (!error.empty())
			std::cerr << "Obj loader error: " << error << std::endl;

		if (!ret) {
			std::cerr << "Raven filed to load .obj\n";
			return std::nullopt;
		}

		//成功读取Obj文件
		std::vector<Point3f> vertices;
		std::vector<Normal3f> normals;
		std::vector<Point2f> uvs;
		std::vector<Vector3f> tangants;
		std::vector<int>indices;

		for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
			const double x = static_cast<const double>(attrib.vertices[v * 3 + 0]);
			const double y = static_cast<const double>(attrib.vertices[v * 3 + 1]);
			const double z = static_cast<const double>(attrib.vertices[v * 3 + 2]);
			vertices.push_back(Point3f(x, y, z));
		}

		for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
			const double x = static_cast<const double>(attrib.normals[v * 3 + 0]);
			const double y = static_cast<const double>(attrib.normals[v * 3 + 1]);
			const double z = static_cast<const double>(attrib.normals[v * 3 + 2]);
			normals.push_back(Normal3f(x, y, z));
		}

		for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
			const double x = static_cast<const double>(attrib.texcoords[v * 2 + 0]);
			const double y = static_cast<const double>(attrib.texcoords[v * 2 + 1]);
			uvs.push_back(Point2f(x, y));
		}

		int faceNum = 0;

		for (size_t s = 0; s < shapes.size(); s++) {
			size_t indexOffset = 0;
			//对于每个面
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				const int verticesNum = shapes[s].mesh.num_face_vertices[f];//这个面有多少个顶点
				for (int i = 0; i < verticesNum; i++) {
					tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + i];//该面第i个顶点的索引信息
					int vertexIndex = idx.vertex_index;
					int normalIndex = idx.normal_index;
					int uvIndex = idx.texcoord_index;
					bool same = vertexIndex == normalIndex && normalIndex == uvIndex;
					//	assert(same);
					indices.push_back(vertexIndex);
				}
				indexOffset += verticesNum;
				faceNum++;
			}
		}

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
		std::cout << "Successfully loaded file: "<<fileName <<", triangle number: " << faceNum <<
			", vertices number: " << vertices.size() << std::endl;;
		return TriangleInfo(faceNum, vertices, normals, uvs, tangants, indices);
	}

	Vector3f toVector(const unsigned char* data) {
		return Vector3f(data[0] / 256.0, data[1] / 256.0, data[2] / 256.0);
	}



	//std::unique_ptr<Vector3f[]> ReadImageVector(const std::string& path, Point2i* resolution) {
	//	const char* filename = (char*)path.c_str();
	//	int w, h, c;
	//	unsigned char* data = stbi_load(filename, &w, &h, &c, 0);
	//	*resolution = Point2i(w, h);
	//	std::unique_ptr<Vector3f[]> convertedData = std::make_unique<Vector3f[]>(w * h);
	//	//TODO::未读取到图片
	//	//TODO::判断图片的通道数
	//	//如果图片为三通道
	//	for (int i = 0; i < w * h; i++) {
	//		convertedData[i] = toVector(&data[i * 3]);

	//	}
	//	stbi_image_free(data);
	//	return convertedData;
	//}


}