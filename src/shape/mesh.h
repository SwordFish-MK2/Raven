#ifndef _RAVEN_SHAPE_MESH_H_
#define _RAVEN_SHAPE_MESH_H_

#include<vector>
#include"../core/base.h"	
#include"../core/shape.h"
#include"../core/transform.h"
#include"../core/math.h"
#include"../core/accelerate.h"
#include"../accelerate/kdTree.h"
namespace Raven {
	struct TriangleInfo {
		int numbers;
		std::vector<Point3f> vertices;
		std::vector<Normal3f> normals;
		std::vector<Point2f> uvs;
		std::vector<Vector3f> tangants;
		std::vector<int>indices;

		TriangleInfo(int num, const std::vector<Point3f>& vs, const std::vector<Normal3f>& ns,
			const std::vector<Point2f>& uvs, const std::vector<Vector3f>& tans, const std::vector<int>& ins) :
			numbers(num), vertices(vs), normals(ns), uvs(uvs), tangants(tans), indices(ins) {}


	};
	/// <summary>
	/// Triangle mesh classes stores all infomation about triangles inside and hold the instances of cordinate triangle array
	/// </summary>
	class TriangleMesh {
	public:
		int nTriangles;
		int nVertices;
		std::vector<Point3f> vertices;//store all vertices in one array to save memory
		std::vector<Normal3f> normals;
		std::vector<Vector3f> tangants;//alternative
		std::vector<Point2f> uvs;//alternative

		std::vector<int> indices;//indices of triangle indices of ith triangle are indices[3*i] to indices[3*i+2]

		bool hasUV;
		bool hasTan;

		TriangleMesh(
			const Transform* OTW, 
			const Transform* WTO, 
			int triNum, 
			const std::vector<Point3f>& vs,
			const std::vector<int>& ins, 
			const std::vector<Normal3f>& ns, 
			const std::vector<Vector3f>& ts,
			const std::vector<Point2f> uvs,
			AccelType buildType = AccelType::KdTree) 
			:OTW(OTW), WTO(WTO),
			nTriangles(triNum), nVertices(vs.size()), vertices(vs), indices(ins), normals(ns), tangants(ts),
			uvs(uvs), hasUV(uvs.size() > 0), hasTan(tangants.size() > 0) {
			//transform all vertices of triangle mesh to world space 
			for (int i = 0; i < vertices.size(); i++) {
				vertices[i] = (*OTW)(vertices[i]);
			}
		}

		std::vector<std::shared_ptr<Triangle>> getTriangles();

		std::vector<std::shared_ptr<Primitive>> generatePrimitive(const std::shared_ptr<Material>& mate,
			const std::shared_ptr<Light>& light = nullptr);

		static std::shared_ptr<TriangleMesh> build(const Transform* WTL,
			const Transform* LTW, const TriangleInfo& info, AccelType buildType = AccelType::KdTree);

	private:
		const Transform* OTW;
		const Transform* WTO;
	};

	class Triangle :public Shape {
	private:
		const TriangleMesh* mesh;
		int i;//此处记录的为三角形第一个index在indices中的位置，构造函数中输入的为三角形在三角形数组中的位置
	public:
		Triangle(const Transform* LTW, const Transform* WTL, const TriangleMesh* m, int index) :
			Shape(LTW, WTL), mesh(m), i(3 * index) {}

		virtual bool hit(const Ray& r_in, double tMax = std::numeric_limits<double>::max())const;

		virtual bool intersect(const Ray& r_in, SurfaceInteraction& record,
			double tMax = std::numeric_limits<double>::max())const;

		Bound3f localBound()const;

		Bound3f worldBound()const;

		double area()const;

		void getUVs(Point2f uv[3])const;

		Point3f getVertex(int num)const {
			return mesh->vertices[index(i)];
		}

		inline int index(int num)const {
			//TODO::检查num的值
			return mesh->indices[i + num];
		}

		std::tuple<SurfaceInteraction, double> sample(const Point2f& rand)const;

		double pdf()const { return 1 / area(); }
	};

	inline std::ostream& operator<<(std::ostream& os, const Triangle& t) {
		const Point3f& p0 = t.getVertex(0);
		const Point3f& p1 = t.getVertex(1);
		const Point3f& p2 = t.getVertex(2);
		os << "[ " << p0 << ", " << p1 << ", " << p2 << " ]";
		return os;
	}

	TriangleMesh CreatePlane(const Transform* LTW, const Transform* WTL, const Point3f& v0,
		const Point3f& v1, const Point3f& v2, const Point3f& v3, const Normal3f& normal);

}

#endif
