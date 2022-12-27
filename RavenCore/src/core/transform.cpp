#include<Raven/core/transform.h>

namespace Raven {

#define Radiance M_PI/180.f

	Transform Translate(const Vector3f& t) {
		Eigen::Matrix4f m;
		m << 1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1;
		return Transform(m);
	}

	Transform Scale(const Vector3f& s) {
		Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
		for (int i = 0; i < 3; i++)
			m(i, i) = s[i];
		return Transform(m);
	}

	Transform Inverse(const Transform& T) {
		return Transform(T.getInverseMatrix());
	}

	Transform Rotate(double angle, const Vector3f& axis) {
		double sinTheta = sin(angle * Radiance);
		double cosTheta = cos(angle * Radiance);
		Eigen::Matrix4f m;
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
		Eigen::Matrix4f m;
		m << cosTheta, sinTheta, 0.f, 0.f,
			-sinTheta, cosTheta, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f;
		return Transform(m);
	}

	Transform RotateX(double angle) {
		double cosTheta = cos(angle * Radiance);
		double sinTheta = sin(angle * Radiance);
		Eigen::Matrix4f m;
		m << 1.f, 0.f, 0.f, 0.f,
			0.f, cosTheta, sinTheta, 0.f,
			0.f, -sinTheta, cosTheta, 0.f,
			0.f, 0.f, 0.f, 1.f;
		return Transform(m);
	}

	Transform RotateY(double angle) {
		double cosTheta = cos(angle * Radiance);
		double sinTheta = sin(angle * Radiance);
		Eigen::Matrix4f m;
		m << cosTheta, 0.f, -sinTheta, 0.f,
			0.f, 1.f, 0.f, 0.f,
			sinTheta, 0.f, cosTheta, 0.f,
			0.f, 0.f, 0.f, 1.f;
		return Transform(m);
	}

	Transform Transform::inverse()const {
		return Transform(invm, m);
	}

	Normal3f Transform::operator()(const Normal3f& n)const {
		double x = n.x, y = n.y, z = n.z;
		return Normal3f(invm(0, 0) * x + invm(1, 0) * y + invm(2, 0) * z,
			invm(0, 1) * x + invm(1, 1) * y + invm(2, 1) * z,
			invm(0, 2) * x + invm(1, 2) * y + invm(2, 2) * z);
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

	Vector3f Transform::operator()(const Vector3f& v)const {
		Eigen::Vector4f nv(v[0], v[1], v[2], 0.0f);
		nv = m * nv;
		return Vector3f(nv(0), nv(1), nv(2));
	}

	Point3f Transform::operator()(const Point3f& p)const {
		Eigen::Vector4f tp(p[0], p[1], p[2], 1.0f);
		tp = m * tp;
		double k = 1 / tp(3);
		return Point3f(tp(0) * k, tp(1) * k, tp(2) * k);
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

	Transform Identity() {
		return Transform(Eigen::Matrix4f::Identity());
	}

	//define the transform from the world to camera space
	Transform LookAt(const Point3f& pos, const Point3f& look, const Vector3f& up) {
		Vector3f viewDir = Normalize(look - pos);
		Vector3f z = viewDir;
		Vector3f right = Normalize(Cross(up, z));
		Vector3f newUp = Normalize(Cross(z, right));
		Eigen::Matrix4f CameraToWorld;
		CameraToWorld << right[0], newUp[0], z[0], pos[0],
			right[1], newUp[1], z[1], pos[1],
			right[2], newUp[2], z[2], pos[2],
			0.0f, 0.0f, 0.0f, 1.0f;
		return Transform(CameraToWorld, CameraToWorld.inverse());
	}

	Transform Perspective(double fov, double znear, double zfar) {
		Eigen::Matrix4f projection;
		projection << 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, zfar / (zfar - znear), -znear * zfar / (zfar - znear),
			0, 0, 1, 0;
		double radiance = M_PI / 180.f;
		double InvTanY = 1 / tan(fov / 2 * radiance);
		return Scale(Vector3f(InvTanY, InvTanY, 1)) * Transform(projection);
	}

	//Transform Orthographic(double top, double bottom, double left, double right, double near, double far) {
	//	//double centerX = (right + left) * 0.5;
	//	//double centerY = (top + bottom) * 0.5;

	//	Transform T = Translate(Vector3f(0, 0, -near));
	//	double scaleX = 1 / abs(right - left);
	//	double scaleY = 1 / abs(top - bottom);
	//	double scaleZ = 1 / abs(far - near);
	//	Transform S = Scale(Vector3f(scaleX, scaleY, scaleZ));

	//	return S * T;
	//}

	Transform Orthographic(double znear, double zfar) {
		return Scale(Vector3f(1, 1, 1 / (zfar - znear))) * Translate(Vector3f(0, 0, -znear));
	}

	//�����Screen space��Raster space֮��ı任
	Transform Raster(int w, int h) {
		//�Ƚ����еĵ�����ֱ��ʵ�һ�룬��תy�ᣬ�ٽ�ͼ�����Ͻ�ƽ�Ƶ�ԭ��
		Eigen::Matrix4f screenToRaster;
		int half_height = h / 2;
		int half_width = w / 2;
		screenToRaster << half_width, 0, 0, (double)half_width,
			0, -half_height, 0, (double)half_height,
			0, 0, 1, 0,
			0, 0, 0, 1;
		return Transform(screenToRaster);
	}
}