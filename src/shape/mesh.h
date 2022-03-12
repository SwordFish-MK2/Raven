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

	/// <summary>
	/// Triangle mesh classes stores all infomation about triangles inside and hold the instances of cordinate triangle array
	/// </summary>
	class TriangleMesh :public Shape {
	public:
		int nTriangles;
		int nVertices;
		std::vector<Point3f> vertices;//store all vertices in one array to save memory
		std::vector<int> indices;//indices of triangle indices of ith triangle are indices[3*i] to indices[3*i+2]
		std::vector<Normal3f> normals;
		std::vector<Vector3f> tangants;//alternative
		std::vector<Point2f> uvs;//alternative
		bool hasUV;
		bool hasTan;

		TriangleMesh(const Transform* OTW, const Transform* WTO, int triNum, const std::vector<Point3f>& vs,
			const std::vector<int>& ins, const std::vector<Normal3f>& ns, const std::vector<Vector3f>& ts,
			const std::vector<Point2f> uvs) :Shape(OTW, WTO), nTriangles(triNum), nVertices(vs.size()),
			vertices(vs), indices(ins), normals(ns), tangants(ts), uvs(uvs), hasUV(uvs.size() > 0), hasTan(tangants.size() > 0) {
			//transform all vertices of triangle mesh to world space 
			for (int i = 0; i < vertices.size(); i++) {
				vertices[i] = (*localToWorld)(vertices[i]);
			}
			buildTriangles();//build triangles
		}

		//copy triangle mesh, call buildTriangles to create kdtree
		TriangleMesh(const TriangleMesh& mesh) :Shape(mesh.localToWorld, mesh.worldToLocal),
			nTriangles(mesh.nTriangles), nVertices(mesh.nVertices), vertices(mesh.vertices),
			indices(mesh.indices), normals(mesh.normals), tangants(mesh.tangants), uvs(mesh.uvs),
			hasTan(mesh.hasTan), hasUV(mesh.hasUV) {
			buildTriangles();
		}
		~TriangleMesh() {
			if (triangles)
				delete triangles;
		}

		bool hit(const Ray& r_in, double tMin = 0.0001, double tMax = std::numeric_limits<double>::max())const;
		bool intersect(const Ray& r_in, SurfaceInteraction& its, double tMin = 0.0001, 
			double tMax = std::numeric_limits<double>::max())const;
		Bound3f localBound()const;
		Bound3f worldBound()const;
		double area()const;
		double pdf()const { return 1 / area(); }
		SurfaceInteraction sample(const Point2f& uv)const;
	private:
		KdTreeAccel* triangles;
		double surfaceArea;
		void buildTriangles();

		//UNTESTED
		//将属于该网格的三角形数据保存在网格类中，随着网格的析构释放三角形的内存
		std::vector<std::shared_ptr<Triangle>> triMemory;
	};

	class Triangle :public Shape {
	private:
		const TriangleMesh* mesh;
		int i;//此处记录的为三角形第一个index在indices中的位置，构造函数中输入的为三角形在三角形数组中的位置
	public:
		Triangle(const Transform* LTW, const Transform* WTL, const TriangleMesh* m, int index) :
			Shape(LTW, WTL), mesh(m), i(3 * index) {}
		bool hit(const Ray& r_in, double tMin = 0.0001, double tMax = std::numeric_limits<double>::max())const;
		bool intersect(const Ray& r_in, SurfaceInteraction& its, double tMin = 0.0001,
			double tMax = std::numeric_limits<double>::max())const;
		Bound3f localBound()const;
		Bound3f worldBound()const;
		double area()const;
		double pdf()const { return 1 / area(); }
		void getUVs(Point2f uv[3])const;
		inline int index(int num)const {
			//TODO::检查num的值
			return mesh->indices[i + num];
		}
		SurfaceInteraction sample(const Point2f& uv)const;
	};

}

#endif
