#include"mesh.h"
#include"../core/distribution.h"
namespace Raven {
	void TriangleMesh::buildTriangles() {
		std::vector<Primitive> prims;
		//surfaceArea = 0;
		for (int i = 0; i < nTriangles; i++) {
			//TODO:Store triangle ptr
			Shape* triangle = new Triangle(localToWorld, worldToLocal, this, i);
			//surfaceArea += triangle->area();
			Primitive p(triangle, nullptr);
			//triangles.push_back(triangle);
			//PtrCollecter::getShape(triangle);
			prims.push_back(p);
		}
		triangles = new KdTreeAccel(prims, -1, 80, 1, 0.5, prims.size());
	}

	bool TriangleMesh::hit(const Ray& r_in, double tMin, double tMax)const {
		return triangles->hit(r_in, tMin, tMax);
	}

	bool TriangleMesh::intersect(const Ray& r_in, SurfaceInteraction& its, double tMin, double tMax)const {
		return triangles->intersect(r_in, its, tMin, tMax);
	}

	Bound3f TriangleMesh::localBound()const {
		return (*worldToLocal)(worldBound());
	}

	Bound3f TriangleMesh::worldBound()const {
		Bound3f box;
		for (size_t i = 0; i < vertices.size(); i++)
			box = Union(box, vertices[i]);
		return box;
	}

	double TriangleMesh::area()const {
		return surfaceArea;
	}

	SurfaceInteraction TriangleMesh::sample(const Point2f& uv)const {
		double allArea = 0;
		double p = GetRand() * surfaceArea;
		for (int i = 0; i < nTriangles; i++) {
			allArea += triangles->prims[i].getShape()->area();
			if (allArea >= p) {
				return triangles->prims[i].getShape()->sample(uv);
			}
		}
	}

	void Triangle::getUVs(Point2f uv[3])const {
		if (mesh->hasUV) {
			//if triangle
			uv[0] = mesh->uvs[mesh->indices[index]];
			uv[1] = mesh->uvs[mesh->indices[index + 1]];
			uv[2] = mesh->uvs[mesh->indices[index + 2]];
		}
		else {
			uv[0] = Point2f(0, 0);
			uv[1] = Point2f(0, 1);
			uv[2] = Point2f(1, 1);
		}
	}

	bool Triangle::hit(const Ray& r_in, double tmin, double tMax)const {
		Point3f p0 = mesh->vertices[mesh->indices[index]];
		Point3f p1 = mesh->vertices[mesh->indices[index + 1]];
		Point3f p2 = mesh->vertices[mesh->indices[index + 2]];
		//transform the ray and triangle that the ray origin lies in the origin
		Point3f ori = r_in.origin;
		Vector3f dire = r_in.dir;
		p0 = Point3f(p0 - r_in.origin);
		p1 = Point3f(p1 - r_in.origin);
		p2 = Point3f(p2 - r_in.origin);
		//find the longest dimention of the ray direction vector and permute it to z
		int z = MaxDimention(dire);
		int x = (z + 1) % 3;
		int y = (x + 1) % 3;
		dire = Vector3f(dire[x], dire[y], dire[z]);
		p0 = Point3f(p0[x], p0[y], p0[z]);
		p1 = Point3f(p1[x], p1[y], p1[z]);
		p2 = Point3f(p2[x], p2[y], p2[z]);
		//shear x,y dimentions that the ray direction aligns with z+ direction
		double XOverZ = dire.x / dire.z;
		double YOverZ = dire.y / dire.z;
		double invertZ = 1.0f / dire.z;
		dire.x = 0.f;
		dire.y = 0.f;
		dire.z = 0.f;
		p0 = Point3f(p0.x - p0.z * XOverZ, p0.y - YOverZ * p0.z, p0.z * invertZ);
		p1 = Point3f(p1.x - p1.z * XOverZ, p1.y - YOverZ * p1.z, p1.z * invertZ);
		p2 = Point3f(p2.x - p2.z * XOverZ, p2.y - YOverZ * p2.z, p2.z * invertZ);
		//test if coordinate(0,0) is in the projected triangle
		double e[3];
		Vector3f p01 = p0 - p1;
		Vector3f p12 = p1 - p2;
		Vector3f p20 = p2 - p0;
		e[0] = -p01.x * p0.y + p01.y * p0.x;//z value of the cross product of two vector, if ei>0,point is on the right side of the vector
		e[1] = -p12.x * p1.y + p12.y * p1.x;
		e[2] = -p20.x * p2.y + p20.y * p2.x;

		if ((e[0] < 0 || e[1] < 0 || e[2] < 0) && (e[0] > 0 || e[1] > 0 || e[2] > 0))
			return false;
		//compute barycentric coodinate
		double tScaled = p0.z * e[0] + p1.z * e[1] + p2.z * e[2];
		double det = e[0] + e[1] + e[2];
		if (det == 0)
			return false;
		if (tScaled > det * tMax) {
			//t>tmax
			return false;
		}
		return true;
	}

	bool Triangle::intersect(const Ray& r_in, SurfaceInteraction& its, double tmin, double tmax)const {
		//取得三角形的三个顶点
		Point3f p0 = mesh->vertices[mesh->indices[index * 3]];
		Point3f p1 = mesh->vertices[mesh->indices[index * 3 + 1]];
		Point3f p2 = mesh->vertices[mesh->indices[index * 3 + 2]];
		//将顶点进行变换使得光线的出发点位于坐标系原点
		Point3f ori = r_in.origin;
		Vector3f dire = r_in.dir;
		Point3f pt0 = Point3f(p0 - r_in.origin);
		Point3f pt1 = Point3f(p1 - r_in.origin);
		Point3f pt2 = Point3f(p2 - r_in.origin);
		//找到光线direction向量的最大分量并将其置换至z轴
		int z = MaxDimention(dire);
		int x = (z + 1) % 3;
		int y = (x + 1) % 3;
		dire = Vector3f(dire[x], dire[y], dire[z]);
		pt0 = Point3f(pt0[x], pt0[y], pt0[z]);
		pt1 = Point3f(pt1[x], pt1[y], pt1[z]);
		pt2 = Point3f(pt2[x], pt2[y], pt2[z]);
		//消去x,y轴，变换后光线从原点出发沿z轴正方向传播
		double XOverZ = -dire.x / dire.z;
		double YOverZ = -dire.y / dire.z;
		double invertZ = 1.0 / dire.z;
		pt0.x += pt0.z * XOverZ;
		pt0.y += pt0.z * YOverZ;
		pt1.x += pt1.z * XOverZ;
		pt1.y += pt1.z * YOverZ;
		pt2.x += pt2.z * XOverZ;
		pt2.y += pt2.z * YOverZ;
		//p0 = Point3f(p0.x - p0.z * XOverZ, p0.y - YOverZ * p0.z, p0.z * invertZ);
		//p1 = Point3f(p1.x - p1.z * XOverZ, p1.y - YOverZ * p1.z, p1.z * invertZ);
		//p2 = Point3f(p2.x - p2.z * XOverZ, p2.y - YOverZ * p2.z, p2.z * invertZ);

		//在2D平面内测试原点(0,0)是否在投影后的三角形内
		double e[3];
		//Vector3f e01 = p0 - p1;
		//Vector3f e12 = p1 - p2;
		//Vector3f e20 = p2 - p0;
		e[0] = pt0.x * pt1.y - pt0.y * pt1.x;//z value of the cross product of two vector, if e[i] > 0, point is on the right side of the vector
		e[1] = pt1.x * pt2.y - pt1.y * pt2.x;
		e[2] = pt2.x * pt0.y + pt2.y * pt0.x;

		if ((e[0] < 0 || e[1] < 0 || e[2] < 0) && (e[0] > 0 || e[1] > 0 || e[2] > 0))
			return false;
		double det = e[0] + e[1] + e[2];
		if (det == 0)
			return false;

		pt0.z *= invertZ;
		pt1.z *= invertZ;
		pt2.z *= invertZ;
		double tScaled = e[0] * p2.z + e[1] * p0.z + e[2] * p1.z;
		if (det < 0 && (tScaled >= 0 || tScaled < tmax * det))
			return false;
		else if (det > 0 && (tScaled <= 0 || tScaled > tmax * det))
			return false;


		double b[3];//barycentric coordinate
		double invDet = 1.f / det;
		b[0] = e[0] * invDet;
		b[1] = e[1] * invDet;
		b[2] = e[2] * invDet;
		double tHit = tScaled * invDet;
		//compute dpdu and dpdv
		Point2f uv[3];
		getUVs(uv);
		Vector3f dpdu;
		Vector3f dpdv;
		Vector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
		Vector3f dp02 = p0 - p2, dp12 = p1 - p2;
		//	double determinate = (uv[0][0] - uv[2][0]) * (uv[1][1] - uv[2][1]) - (uv[0][1] - uv[2][1]) * (uv[1][0] - uv[2][0]);
		double determinate = duv02[0] * duv12[1] - duv02[1] * duv12[0];
		if (determinate == 0)
			genTBN(Normalize(Cross(p2 - p0, p1 - p0)), &dpdu, &dpdv);
		else {
			double invertDet = 1.0 / determinate;
			Vector3f p02 = p0 - p2;
			Vector3f p12 = p1 - p2;
			dpdu = ( duv12[1] * dp02 - duv02[1] * dp12) * invertDet;
			dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invertDet;
		}
		//compute pHit and uvHit
		Point3f pHit = b[0] * p0 + b[1] * p1 + b[2] * p2;
		Point2f uvHit = b[0] * uv[0] + b[1] * uv[1] + b[2] * uv[2];
		//generate surfaceIntersection
		its.n = Normalize(Cross(dp02, dp12));
		its.p = pHit;
		its.uv = uvHit;
		its.dpdu = dpdu;
		its.dpdv = dpdv;
		its.dndu = its.dndv = Vector3f(0.f, 0.f, 0.f);
		return true;
	}

	Bound3f Triangle::localBound()const {
		Bound3f box;
		Point3f p0 = mesh->vertices[mesh->indices[index]];
		Point3f p1 = mesh->vertices[mesh->indices[index + 1]];
		Point3f p2 = mesh->vertices[mesh->indices[index + 2]];
		p0 = (*worldToLocal)(p0);
		p1 = (*worldToLocal)(p1);
		p2 = (*worldToLocal)(p2);
		box.pMin = Point3f(Min(p0.x, p1.x, p2.x), Min(p0.y, p1.y, p2.y), Min(p0.z, p1.z, p2.z));
		box.pMax = Point3f(Max(p0.x, p1.x, p2.x), Max(p0.y, p1.y, p2.y), Max(p0.z, p1.z, p2.z));
		return box;
	}

	Bound3f Triangle::worldBound()const {
		Bound3f box;
		Point3f p0 = mesh->vertices[mesh->indices[index]];
		Point3f p1 = mesh->vertices[mesh->indices[index + 1]];
		Point3f p2 = mesh->vertices[mesh->indices[index + 2]];
		box.pMin = Point3f(Min(p0.x, p1.x, p2.x), Min(p0.y, p1.y, p2.y), Min(p0.z, p1.z, p2.z));
		box.pMax = Point3f(Max(p0.x, p1.x, p2.x), Max(p0.y, p1.y, p2.y), Max(p0.z, p1.z, p2.z));
		return box;
	}

	double Triangle::area()const {
		return 0.f;
	}

	SurfaceInteraction Triangle::sample(const Point2f& uv)const {
		//get barycentric coordinate
		Point2f b = UniformSampleTriangle(uv);

		//get vertices of triangle
		int i0 = mesh->indices[index * 3];
		int i1 = mesh->indices[index * 3 + 1];
		int i2 = mesh->indices[index * 3 + 2];

		Point3f p0 = mesh->vertices[i0];
		Point3f p1 = mesh->vertices[i1];
		Point3f p2 = mesh->vertices[i2];

		Normal3f n0 = mesh->normals[i0];
		Normal3f n1 = mesh->normals[i1];
		Normal3f n2 = mesh->normals[i2];

		//interpolate vertices by barycentric coordinate
		Point3f sample = p0 * b[0] + p1 * b[1] + p2 * (1 - b[0] - b[1]);
		Normal3f normal = n0 * b[0] + n1 * b[1] + n2 * (1 - b[0] - b[1]);

		SurfaceInteraction sisec;
		sisec.n = normal;
		sisec.p = sample;
		return sisec;
	}

}