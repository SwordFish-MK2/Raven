#include<Raven/core/transform.h>

namespace Raven {
#define Radiance M_PI/180.f

	Mat4f Mat4f::inverse()const {
		Mat4f mat = *this;
		Mat4f invMat{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };

		Float pivot[4] = { 0,0,0,0 };

		//高斯消元
		Float singular = false;

		//向下消元,将矩阵化为上三角矩阵
		for (size_t i = 0; i < 4; i++) {
			pivot[i] = mat(i, i);//当前列的pivot

			//主元为0时，向下寻找一行交换
			if (pivot[i] == 0) {

				//向下寻找一行交换
				for (size_t j = i + 1; j < 4; j++) {
					singular = true;
					if (mat(j, i) != 0) {
						//找非0 pivot，交换两行
						for (size_t k = 0; k < 4; k++) {
							std::swap(mat.data[j * 4 + k], mat.data[i * 4 + k]);
							std::swap(invMat.data[j * 4 + k], mat.data[i * 4 + k]);
						}
						pivot[i] = mat(j, i);
						singular = false;
						break;
					}
				}

				//奇异矩阵，不可求逆
				assert(!singular, "Tried to invert a singular Matrix.");
			}

			//将主元化为1
			Float invPivot = 1.0 / pivot[i];
			for (size_t j = 0; j < 4; j++) mat(i, j) *= invPivot;
			for (size_t j = 0; j < 4; j++) invMat(i, j) *= invPivot;

			//向下消元
			for (size_t j = i + 1; j < 4; j++) {
				Float mul = mat(j, i);//当前需要消去的数为主元的mul倍
				if (mul == 0)continue;
				for (size_t k = 0; k < 4; k++) {
					mat(j, k) -= mul * mat(i, k);
					invMat(j, k) -= mul * invMat(i, k);
				}
			}
		}

		//向上反向消元
		for (int i = 0; i < 4; i++) {
			for (int j = i - 1; j >= 0; j--) {
				Float mul = mat(j, i);
				if (mul == 0)continue;
				for (int k = 0; k < 4; k++) {
					mat(j, k) -= mul * mat(i, k);
					invMat(j, k) -= mul * invMat(i, k);
				}
			}
		}

		//将对角线化为0
		for (size_t i = 0; i < 4; i++) {
			Float invMul = 1.0 / mat(i, i);
			for (size_t j = 0; j < 4; j++) invMat(i, j) *= invMul;
		}

		return invMat;
	}

	template<class T>
	Vector3<T> Transform::operator()(const Vector3<T>& v)const {
		return Vector3<T>{ v.x* m.data[0] + v.y * m.data[1] + v.z * m.data[2],
			v.x* m.data[4] + v.y * m.data[5] + v.z * m.data[6],
			v.x* m.data[8] + v.y * m.data[9] + v.z * m.data[10] };
	}

	template<class T>
	Point3<T> Transform::operator()(const Point3<T>& p)const {
		Point3<T> p2{ p.x * m.data[0] + p.y * m.data[1] + p.z * m.data[2] + m.data[3],
			p.x * m.data[4] + p.y * m.data[5] + p.z * m.data[6] + m.data[7],
			p.x * m.data[8] + p.y * m.data[9] + p.z * m.data[10] + m.data[11] };
		T w = p.x * m.data[12] + p.y * m.data[13] + p.z * m.data[14] + m.data[15];

		//三维点的w分量必须为1
		if (w == 1)return p2;
		else return p2 /= w;
	}

	template<class T>
	Normal3<T> Transform::operator()(const Normal3<T>& v)const {
		return Normal3<T>{v.x* invm.data[0] + v.y * invm.data[4] + v.z * invm.data[8],
			v.x* invm.data[1] + v.y * invm.data[5] + v.z * invm.data[9],
			v.x* invm.data[2] + v.y * invm.data[6] + v.z * invm.data[10]};
	}

	Bound3f Transform::operator()(const Bound3f& b)const {
		const Transform& m = *this;
		Bound3f ret(m(Point3f(b.pMax.x, b.pMax.y, b.pMax.z)));
		ret = Union(ret, m(Point3f(b.pMax.x, b.pMax.y, b.pMin.z)));
		ret = Union(ret, m(Point3f(b.pMax.x, b.pMin.y, b.pMin.z)));
		ret = Union(ret, m(Point3f(b.pMin.x, b.pMin.y, b.pMin.z)));
		ret = Union(ret, m(Point3f(b.pMin.x, b.pMin.y, b.pMax.z)));
		ret = Union(ret, m(Point3f(b.pMin.x, b.pMax.y, b.pMax.z)));
		ret = Union(ret, m(Point3f(b.pMin.x, b.pMax.y, b.pMin.z)));
		ret = Union(ret, m(Point3f(b.pMax.x, b.pMin.y, b.pMax.z)));
		return ret;
	}

	Ray Transform::operator()(const Ray& r)const {
		Point3f p = (*this)(r.origin);
		Vector3f d = (*this)(r.dir).normalized();
		return Ray(p, d, r.t);
	}

	RayDifferential Transform::operator()(const RayDifferential& rd)const {
		Point3f p = (*this)(rd.origin);
		Vector3f d = Normalize((*this)(rd.dir));
		RayDifferential rt(p, d, rd.t);
		if (rd.hasDifferential) {
			rt.hasDifferential = true;

			rt.originX = (*this)(rd.originX);
			rt.originY = (*this)(rd.originY);

			rt.directionX = Normalize((*this)(rd.directionX));
			rt.directionY = Normalize((*this)(rd.directionY));
		}
		return rt;
	}

	SurfaceInteraction Transform::operator()(const SurfaceInteraction& its)const {
		SurfaceInteraction inter;
		const Transform& t = *this;
		inter.p = t(its.p);
		inter.dndu = t(its.dndu);
		inter.dndv = t(its.dndv);
		inter.dpdu = t(its.dpdu);
		inter.dpdv = t(its.dpdv);
		inter.n = Normalize(t(its.n));
		inter.wo = Normalize(t(its.wo));
		inter.bsdf = its.bsdf;
		inter.uv = its.uv;
		inter.t = its.t;
		inter.hitLight = its.hitLight;
		inter.light = its.light;
		return inter;
	}

	Transform Transform::operator*(const Transform& t2)const {
		return Transform(m * t2.m, invm * t2.invm);
	}



	Transform Transform::inverse()const {
		return Transform(invm, m);
	}

	Transform Rotate(double angle, const Vector3f& axis) {
		double sinTheta = sin(angle * Radiance);
		double cosTheta = cos(angle * Radiance);
		Mat4f m;
		m << axis[0] * axis[0] * (1 - cosTheta) + cosTheta,
			axis[0] * axis[1] * (1 - cosTheta) + axis[2] * sinTheta,
			axis[0] * axis[2] * (1 - cosTheta) - axis[1] * sinTheta,
			axis[0] * axis[1] * (1 - cosTheta) - axis[2] * sinTheta,
			axis[0] * axis[1] * (1 - cosTheta) + cosTheta,
			axis[1] * axis[2] * (1 - cosTheta) + axis[2] * sinTheta,
			axis[0] * axis[2] * (1 - cosTheta) + axis[1] * sinTheta,
			axis[1] * axis[2] * (1 - cosTheta) - axis[2] * sinTheta,
			axis[2] * axis[2] * (1 - cosTheta) + cosTheta;
		return Transform(m);
	}

	Transform RotateZ(double angle) {
		double cosTheta = cos(angle * Radiance);
		double sinTheta = sin(angle * Radiance);
		Mat4f m{ cosTheta, sinTheta, 0.f, 0.f,
			-sinTheta, cosTheta, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f };
		return Transform(m);
	}

	Transform RotateX(double angle) {
		double cosTheta = cos(angle * Radiance);
		double sinTheta = sin(angle * Radiance);
		Mat4f m{ 1.f, 0.f, 0.f, 0.f,
			0.f, cosTheta, sinTheta, 0.f,
			0.f, -sinTheta, cosTheta, 0.f,
			0.f, 0.f, 0.f, 1.f };
		return Transform(m);
	}

	Transform RotateY(double angle) {
		double cosTheta = cos(angle * Radiance);
		double sinTheta = sin(angle * Radiance);
		Mat4f m{ cosTheta, 0.f, -sinTheta, 0.f,
				0.f, 1.f, 0.f, 0.f,
				sinTheta, 0.f, cosTheta, 0.f,
				0.f, 0.f, 0.f, 1.f };
		return Transform(m);
	}

	//define the transform from the world to camera space
	Transform LookAt(const Point3f& pos, const Point3f& look, const Vector3f& up) {
		Vector3f viewDir = Normalize(look - pos);
		Vector3f z = viewDir;
		Vector3f right = Normalize(Cross(up, z));
		Vector3f newUp = Normalize(Cross(z, right));

		return Transform(right[0], newUp[0], z[0], pos[0],
			right[1], newUp[1], z[1], pos[1],
			right[2], newUp[2], z[2], pos[2],
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	Transform Perspective(double fov, double znear, double zfar) {
		Mat4f projection{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, zfar / (zfar - znear), -znear * zfar / (zfar - znear),
			0, 0, 1, 0 };
		double radiance = M_PI / 180.f;
		double InvTanY = 1 / tan(fov / 2 * radiance);
		return Scale(Vector3f(InvTanY, InvTanY, 1)) * Transform(projection);
	}

	Transform Orthographic(double znear, double zfar) {
		return Scale(Vector3f(1, 1, 1 / (zfar - znear))) * Translate(Vector3f(0, 0, -znear));
	}

	//负责从Screen space到Raster space之间的变换
	Transform Raster(int w, int h) {
		//先将所有的点拉伸分辨率的一半，反转y轴，再将图像左上角平移到原点

		int half_height = h / 2;
		int half_width = w / 2;
		Mat4f screenToRaster(half_width, 0, 0, (double)half_width,
			0, -half_height, 0, (double)half_height,
			0, 0, 1, 0,
			0, 0, 0, 1);
		return Transform(screenToRaster);
	}
}