#ifndef _RAVEN_SHAPE_MESH_H_
#define _RAVEN_SHAPE_MESH_H_

#include<vector>
#include<Raven/core/base.h>
#include<Raven/core/shape.h>
#include<Raven/core/transform.h>
#include<Raven/core/math.h>
#include<Raven/core/accelerate.h>
#include<Raven/accelerate/kdTree.h>

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
	/// Triangle mesh classes stores all infomation about triangles inside and hold the instances of relative triangle array
	/// </summary>
	class TriangleMesh :public RavenObject {
	public:
		TriangleMesh(
			const Ref<Transform>& OTW, const Ref<Transform>& WTO, int triNum, const std::vector<Point3f>& vs,
			const std::vector<int>& ins, const std::vector<Normal3f>& ns, const std::vector<Vector3f>& ts,
			const std::vector<Point2f> uvs);

		static Ref<TriangleMesh> construct(const PropertyList& param);

	private:
		void generateTriangles();

	public:
		const int nTriangles;
		const int nVertices;
		std::vector<Point3f> vertices;//store all vertices in one array to save memory
		std::vector<Normal3f> normals;
		std::vector<Vector3f> tangants;//alternative
		std::vector<Point2f> uvs;//alternative

		std::vector<int> indices;//indices of triangle indices of ith triangle are indices[3*i] to indices[3*i+2]

		std::vector<std::shared_ptr<Triangle>> triangles;

		bool hasUV;
		bool hasTan;

	private:

		Ref<Transform> OTW;
		Ref<Transform> WTO;
	};

	class Triangle :public Shape {
	public:
		Triangle(const Ref<Transform>& LTW, const Ref<Transform>& WTL, const TriangleMesh* m, int index) :
			Shape(LTW, WTL), mesh(m), i(3 * index) {}

		bool hit(const Ray& r_in, double tMax = std::numeric_limits<double>::max())const override;

		bool intersect(const Ray& r_in, HitInfo& info)const override;

		SurfaceInteraction getGeoInfo(const Point3f& hitInfo)const override;

		Bound3f localBound()const override;

		Bound3f worldBound()const override;

		double area()const override;

		void getUVs(Point2f uv[3])const;

		Point3f getVertex(int num)const {
			return mesh->vertices[index(i)];
		}

		int index(int num)const {
			//TODO::检查num的值
			return mesh->indices[i + num];
		}

		std::tuple<SurfaceInteraction, double> sample(const Point2f& rand)const override;

	private:
		const TriangleMesh* mesh;
		int i;//此处记录的为三角形第一个index在indices中的位置，构造函数中输入的为三角形在三角形数组中的位置
	};

	inline std::ostream& operator<<(std::ostream& os, const Triangle& t) {
		const Point3f& p0 = t.getVertex(0);
		const Point3f& p1 = t.getVertex(1);
		const Point3f& p2 = t.getVertex(2);
		os << "[ " << p0 << ", " << p1 << ", " << p2 << " ]";
		return os;
	}

	//_RAVEN_CLASS_REG_(mesh,TriangleMesh,TriangleMesh::construct)

	std::shared_ptr<TriangleMesh> CreatePlane(const Ref<Transform>& LTW, const Ref<Transform>& WTL, const Point3f& v0,
		const Point3f& v1, const Point3f& v2, const Point3f& v3, const Normal3f& normal);

}

#endif
