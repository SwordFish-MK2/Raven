#ifndef _RAVEN_CORE_TRANSFORM_H_
#define _RAVEN_CORE_TRANSFORM_H_

#define _USE_MATH_DEFINES
#include<math.h>
#include<Eigen/Eigen>
#include"math.h"
#include"base.h"
#include"ray.h"
#include"aabb.h"
#include"interaction.h"

namespace Raven {
	class Transform {
	private:
		Eigen::Matrix4f m, invm;
		//friend struct Quaternion;
		//friend class AnimationTransform;
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			Transform() {
			m = Eigen::Matrix4f::Identity();
			invm = m.inverse();
		}
		Transform(const Eigen::Matrix4f& M) :m(M) { invm = m.inverse(); }
		Transform(double mat[4][4]) {
			m << mat[0][0], mat[0][1], mat[0][2], mat[0][3],
				mat[1][0], mat[1][1], mat[1][2], mat[1][3],
				mat[2][0], mat[2][1], mat[2][2], mat[2][3],
				mat[3][0], mat[3][1], mat[3][2], mat[3][3];
			invm = m.inverse();
		}
		Transform(const Eigen::Matrix4f& M, const Eigen::Matrix4f& invM) :m(M), invm(invM) {}
		Eigen::Matrix4f getMatrix() const { return m; }
		Eigen::Matrix4f getInverseMatrix() const { return invm; }
		//overload operator() to apply transform-----------------------------
		Vector3f operator()(const Vector3f& v)const;
		Point3f operator()(const Point3f& p)const;
		Transform Rotate(double angle, const Vector3f& axis);
		Normal3f operator()(const Normal3f& n)const;
		Bound3f operator()(const Bound3f& b)const;
		Ray operator()(const Ray& r)const;
		RayDifferential operator()(const RayDifferential& rd)const;
		SurfaceInteraction operator()(const SurfaceInteraction& its)const;
		//overload operator* to apply multiplition of transform--------------
		Transform operator*(const Transform& t)const {
			return Transform(m * t.m, invm * t.invm);
		}
		//wheather the coordinate system is changed is determined by wheather the determinant of the uper3*3 matrix < 0
		//bool SwapsHandness()const;
	};

	Transform Identity();
	Transform Rotate(double angle, const Vector3f& axis);
	Transform RotateX(double angle);
	Transform RotateY(double angle);
	Transform RotateZ(double angle);
	Transform Translate(const Vector3f& t);
	Transform Scale(const Vector3f& s);
	Transform Inverse(const Transform& T);
	Transform LookAt(const Point3f& pos, const Point3f& look, const Vector3f& up);
	Transform Orthographic(double top, double bottom, double left, double right, double znear, double zfar);
	Transform Perspective(double fov, double aspect_ratio, double znear, double zfar);
	Transform Raster(int h, int w);
}

#endif