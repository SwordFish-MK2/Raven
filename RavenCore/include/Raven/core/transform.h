#ifndef _RAVEN_CORE_TRANSFORM_H_
#define _RAVEN_CORE_TRANSFORM_H_

#define _USE_MATH_DEFINES

#include<math.h>
#include<Eigen/Dense>
#include<Raven/core/math.h>
#include<Raven/core/base.h>
#include<Raven/core/ray.h>
#include<Raven/core/aabb.h>
#include<Raven/core/interaction.h>
#include<Raven/core/object.h>
#include<assert.h>
#include<optional>

namespace Raven {

	/// <summary>
	/// 矩阵类，使用行向量优先存储
	/// </summary>
	class Mat4f {
	public:
		Mat4f() :index(0) { data = std::array<Float, 16>{0.0}; }

		Mat4f(Float v00, Float v01, Float v02, Float v03,
			Float v10, Float v11, Float v12, Float v13,
			Float v20, Float v21, Float v22, Float v23,
			Float v30, Float v31, Float v32, Float v33) :
			index(0),
			data(std::array<Float, 16>{v00, v01, v02, v03,
				v10, v11, v12, v13,
				v20, v21, v22, v23,
				v30, v31, v32, v33}) {}

		Mat4f(const std::array<Float, 16>& d) :index(0), data(d) {}

		Mat4f& operator<<(Float val) {
			assert(++index < 16);
			data[index] = val;
			return *this;
		}

		Float operator()(int row, int column)const {
			assert(row < 4 && row >= 0);
			assert(column < 4 && column >= 0);
			int i = row * 4 + column;
			return data[i];
		}

		Float& operator()(int row, int column) {
			assert(row < 4 && row >= 0);
			assert(column < 4 && column >= 0);
			int i = row * 4 + column;
			return data[i];
		}

		Mat4f operator*(const Mat4f& m2)const {
			Mat4f mat;
			for (size_t i = 0; i < 4; ++i)
				for (size_t j = 0; j < 4; ++j)
					mat(i, j) = this->data[i * 4 + 0] * m2.data[j] +
					this->data[i * 4 + 1] * m2.data[j + 4] +
					this->data[i * 4 + 2] * m2.data[j + 8] +
					this->data[i * 4 + 3] * m2.data[j + 12];
			return mat;
		}

		template<class T>
		Vector3<T> operator*(const Vector3<T>& v)const {
			Vector3<T> v{ v.x * data[0] + v.y * data[1] + v.z * data[2],
			v.x * data[4] + v.y * data[5] + v.z * data[6],
			v.x * data[8] + v.y * data[9] + v.z * data[10] };
			return v;
		}

		template<class T>
		Point3<T> operator*(const Point3<T>& p)const {
			Point3<T> p{ p.x * data[0] + p.y * data[1] + p.z * data[2] + data[3],
			p.x * data[4] + p.y * data[5] + p.z * data[6] + data[7],
			p.x * data[8] + p.y * data[9] + p.z * data[10] + data[11] };
			T w = p.x * data[12] + p.y * data[13] + p.z * data[14] + data[15];

			//三维点的w分量必须为1
			if (w == 1)return p;
			else return p /= w;
		}

		Mat4f transpose()const {
			Mat4f mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat(i, j) = (*this)(j, i);
			return mat;
		}

		std::optional<Mat4f> inverse()const {
			Mat4f mat = *this;
			Mat4f invMat = Mat4f::identity();

			int pivot[4] = { 0,0,0,0 };

			//高斯消元
			bool singular = false;

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
					if (singular) {
						std::cout << "Singular matrix, can not find inverse.";
						return std::nullopt;
					}
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

		inline friend std::ostream& operator<<(std::ostream& os, const Mat4f& mat);

		static Mat4f identity() {
			return Mat4f{ 1.0, 0.0, 0.0, 0.0,
					0.0, 1.0 ,0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0 ,1.0 };
		}

	private:
		int index;
		std::array<Float, 16> data;
	};

	inline std::ostream& operator<<(std::ostream& os, const Mat4f& mat) {
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 3; j++)
				os << mat(i, j) << ",";
			os << mat(i, 3) << std::endl;
		}
		return os;
	}

	class Transform :public RavenObject {
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
		Transform inverse()const;
		//inline friend void operator<<(std::ostream& os, const Transform& t) {
		//	std::cout << "[";
		//	//for(size_t i=0;i<4;i++)
		//	//	for(si)
		//}

	private:
		Eigen::Matrix4f m, invm;
		//friend struct Quaternion;
		//friend class AnimationTransform;
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
	Transform Orthographic(double near, double far);
	Transform Perspective(double fov, double znear, double zfar);
	Transform Raster(int h, int w);
}

#endif