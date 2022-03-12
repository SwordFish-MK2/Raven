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
		Point3f p0 = mesh->vertices[mesh->indices[index * 3]];
		Point3f p1 = mesh->vertices[mesh->indices[index * 3 + 1]];
		Point3f p2 = mesh->vertices[mesh->indices[index * 3 + 2]];
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
		//取从网格中取出三角形的三个顶点p0,p1,p2
		const Point3f& p0 = mesh->vertices[mesh->indices[index * 3]];
		const Point3f& p1 = mesh->vertices[mesh->indices[index * 3 + 1]];
		const Point3f& p2 = mesh->vertices[mesh->indices[index * 3 + 2]];
		//利用克莱姆法则求解三角型的重心坐标与光线的传播时间t
		Vector3f e1 = p1 - p0;
		Vector3f e2 = p2 - p0;
		Vector3f s = r_in.origin - p0;
		Vector3f s1 = Cross(r_in.dir, e2);
		Vector3f s2 = Cross(s, e1);
		auto determinate = Dot(s1, e1); //
		if (determinate <= 0)return false;
		double invDet = 1.0 / Dot(s1, e1);
		double t = invDet * Dot(s2, e2);
		double b1 = invDet * Dot(s1, s);
		double b2 = invDet * Dot(s2, r_in.dir);
		double b0 = 1 - b1 - b2; if (b0 < 0)return false;
		//光线必须沿正向传播
		if (t <= 0)
			return false;
		//重心坐标都大于0时，交点在三角形内，光线与三角形相交	
		if (b0 <= 0.0 || b1 <= 0.0 || b2 <= 0.0)
			return false;
		//TODO::检查相交时间
		if (t >= tmax)
			return false;

		//利用重心坐标插值求出交点几何坐标、纹理坐标与法线
		//Point2f uv[3];
		//getUVs(uv);
		const Point2f& uv0 = mesh->uvs[mesh->indices[index * 3]];
		const Point2f& uv1 = mesh->uvs[mesh->indices[index * 3 + 1]];
		const Point2f& uv2 = mesh->uvs[mesh->indices[index * 3 + 2]];


		const Normal3f& n0 = mesh->normals[mesh->indices[index * 3]];
		const Normal3f& n1 = mesh->normals[mesh->indices[index * 3 + 1]];
		const Normal3f& n2 = mesh->normals[mesh->indices[index * 3 + 2]];
		Point3f pHit = b0 * p0 + b1 * p1 + b2 * p2;
		Point2f uvHit = b0 * uv0 + b1 * uv1 + b2 * uv2;
		auto nHit = b0 * n0 + b1 * n1 + b2 * n2;
		////TODO:: 确认三角形朝向（顺时针？逆时针？），确保计算出的法线与原法线为同一方向
		//Vector3f nHit = Cross(p0 - p1, p2 - p1);
		////计算dpdu与dpdv以便在网格表面求出的切线向量为连续的值
		Vector3f dpdu, dpdv;
		double du02 = uv0[0] - uv2[0], du12 = uv1[0] - uv2[0];
		double dv02 = uv0[1] - uv2[1], dv12 = uv1[1] - uv2[1];
		double det = du02 * dv12 - dv02 * du12;
		//如果行列式等于0，随机生成一组互相垂直的dpdu与dpdv
		if (det == 0) {
			genTBN((Vector3f)nHit, &dpdu, &dpdv);
		}
		else {
			auto invDet = 1 / det;
			Vector3f dp02 = p0 - p1;
			Vector3f dp12 = p1 - p2;
			dpdu = invDet * (dv12 * dp02 - dv02 * dp12);
			dpdv = invDet * (du02 * dp12 - du12 * dp02);
		}

		its.dpdu = dpdu;
		its.dpdv = dpdv;
		its.n = nHit;
		its.p = pHit;
		its.uv = uvHit;
		return true;


		//const Vector3f& dir = r_in.dir;
		//const Point3f& orig = r_in.origin;
		//Vector3f e1 = p1 - p0;
		//Vector3f e2 = p2 - p0;
		//Vector3f s1 = Cross(dir, e2);
		//float det = Dot(e1, s1);
		//if (det == 0 || det < 0)
		//	return false;

		//Vector3f s = orig - p0;
		//double u = Dot(s, s1);
		//if (u < 0 || u > det)
		//	return false;

		//Vector3f s2 = Cross(s, e1);
		//double v = Dot(s2, dir);
		//if (v < 0 || u + v > det)
		//	return false;

		//float invDet = 1 / det;

		//double t = Dot(s2, e2) * invDet;
		//u *= invDet;
		//v *= invDet;

		//const Point2f& uv0 = mesh->uvs[mesh->indices[index]];
		//const Point2f& uv1 = mesh->uvs[mesh->indices[index + 1]];
		//const Point2f& uv2 = mesh->uvs[mesh->indices[index + 2]];


		//const Normal3f& n0 = mesh->normals[mesh->indices[index]];
		//const Normal3f& n1 = mesh->normals[mesh->indices[index + 1]];
		//const Normal3f& n2 = mesh->normals[mesh->indices[index + 2]];

		//auto pHit = r_in.position(t);
		//auto uvHit = (1 - u - v) * uv0 + u * uv1 + v * uv2;
		//auto nHit = (1 - u - v) * n0 + u * n1 + v * n2;
		//its.n = nHit;
		//its.p = pHit;
		//its.uv = uvHit;
		//return true;
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