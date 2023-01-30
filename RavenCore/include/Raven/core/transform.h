#ifndef _RAVEN_CORE_TRANSFORM_H_
#define _RAVEN_CORE_TRANSFORM_H_

#define _USE_MATH_DEFINES

#include<math.h>
#include<array>
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
	/// �����࣬ʹ�����������ȴ洢
	/// </summary>
	class Mat4f {
	private:
		class CommaWapper {
		public:
			CommaWapper() = delete;
			explicit CommaWapper(Mat4f* const m) :m(m) {}
			CommaWapper& operator,(Float val) {
				m->data[m->index++] = val;
				return *this;
			}
		private:
			Mat4f* const m;
		};
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

		CommaWapper operator<<(Float val) {
			data[index++] = val;
			return CommaWapper(this);
		}

		Float operator()(int row, int column)const {
			int i = row * 4 + column;
			return data[i];
		}

		Float& operator()(int row, int column) {
			int i = row * 4 + column;
			return data[i];
		}

		Mat4f operator*(const Mat4f& m2)const {
			Mat4f mat;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					mat(i, j) = this->data[i * 4 + 0] * m2.data[j] +
					this->data[i * 4 + 1] * m2.data[j + 4] +
					this->data[i * 4 + 2] * m2.data[j + 8] +
					this->data[i * 4 + 3] * m2.data[j + 12];
			return mat;
		}

		template<class T>
		Normal3<T> operator*(const Normal3<T>& n)const {
			Mat4f invm = this->inverse();
			return Normal3<T>{n.x* invm.data[0] + n.y * invm.data[4] + n.z * invm.data[8],
				n.x* invm.data[1] + n.y * invm.data[5] + n.z * invm.data[9],
				n.x* invm.data[2] + n.y * invm.data[6] + n.z * invm.data[10]};
		}

		template<class T>
		Vector3<T> operator*(const Vector3<T>& v)const {
			return Vector3<T>{ v.x * data[0] + v.y * data[1] + v.z * data[2],
			v.x * data[4] + v.y * data[5] + v.z * data[6],
			v.x * data[8] + v.y * data[9] + v.z * data[10] };
		}

		template<class T>
		Point3<T> operator*(const Point3<T>& p)const {
			Point3<T> resultP{ p.x * data[0] + p.y * data[1] + p.z * data[2] + data[3],
			p.x * data[4] + p.y * data[5] + p.z * data[6] + data[7],
			p.x * data[8] + p.y * data[9] + p.z * data[10] + data[11] };
			T w = p.x * data[12] + p.y * data[13] + p.z * data[14] + data[15];

			//��ά���w��������Ϊ1
			if (w == 1)return resultP;
			else return resultP /= w;
		}

		Mat4f transpose()const {
			Mat4f mat;
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					mat.data[i * 4 + j] = this->data[j * 4 + i];
			return mat;
		}

		Mat4f inverse()const;

		inline friend std::ostream& operator<<(std::ostream& os, const Mat4f& mat);

	private:
		int index;


	public:
		friend CommaWapper;
		std::array<Float, 16> data;
	};

	inline std::ostream& operator<<(std::ostream& os, const Mat4f& mat) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 3; j++)
				os << mat(i, j) << ",";
			os << mat(i, 3) << std::endl;
		}
		return os;
	}

	class Transform :public RavenObject {
	public:
		Transform() {}

		Transform(const Mat4f& mat) :m(mat), invm(mat.inverse()) {}

		Transform(const Mat4f& mat, const Mat4f& invMat) :m(mat), invm(invMat) {}

		Transform(const std::array<Float, 16>& data) :m(Mat4f(data)), invm(m.inverse()) {}

		Transform(Float v00, Float v01, Float v02, Float v03,
			Float v10, Float v11, Float v12, Float v13,
			Float v20, Float v21, Float v22, Float v23,
			Float v30, Float v31, Float v32, Float v33) :
			m(Mat4f(v00, v01, v02, v03, v10, v11, v12, v13, v20, v21, v22, v23, v30, v31, v32, v33)),
			invm(m.inverse()) {}

		template<class T>
		Vector3<T> operator()(const Vector3<T>& v)const;

		template<class T>
		Point3<T> operator()(const Point3<T>& p)const;

		template<class T>
		Normal3<T> operator()(const Normal3<T>& n)const;

		Bound3f operator()(const Bound3f& box)const;

		Ray operator()(const Ray& r)const;

		RayDifferential operator()(const RayDifferential& r)const;

		SurfaceInteraction operator()(const SurfaceInteraction& sinter)const;

		Transform operator*(const Transform& t2)const;

		Transform inverse()const;

		//bool swapHandness()const;

		inline friend std::ostream& operator<<(std::ostream&, const Transform& t);

	private:
		Mat4f m, invm;
	};

	inline std::ostream& operator<<(std::ostream& os, const Transform& t) {
		os << "t =\n" << t.m << "inv =\n" << t.invm;
		return os;
	}

	inline Transform Inverse(const Transform& t) { return t.inverse(); }
	inline Transform Identity() {
		Mat4f m{ 1.0, 0.0 ,0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0 };
		return Transform(m, m);
	}
	inline Transform Translate(const Vector3f& t) {
		Mat4f m{ 1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1 };
		Mat4f invm{
			1, 0, 0, -t.x,
			0, 1, 0, -t.y,
			0, 0, 1, -t.z,
			0, 0, 0, 1
		};
		return Transform(m, invm);
	}
	inline Transform Translate(Float x, Float y, Float z) {
		Mat4f m{
			1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1 };
		Mat4f invm{
			1, 0, 0, -x,
			0, 1, 0, -y,
			0, 0, 1, -z,
			0, 0, 0, 1 };
		return Transform(m, invm);
	}
	inline Transform Scale(const Vector3f& s) {
		Mat4f m{
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		};
		Mat4f invm{ 1.0 / s.x, 0, 0, 0,
			0, 1.0 / s.y, 0, 0,
			0, 0, 1.0 / s.z, 0,
			0, 0, 0, 1
		};
		return Transform(m);
	}
	inline Transform Scale(Float x, Float y, Float z) {
		Mat4f m{ x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1 };
		Mat4f invm{ 1.0 / x, 0, 0, 0,
			0, 1.0 / y, 0, 0,
			0, 0, 1.0 / z, 0,
			0, 0, 0, 1
		};
		return Transform(m, invm);
	}

	Transform Rotate(double angle, const Vector3f& axis);
	Transform RotateX(double angle);
	Transform RotateY(double angle);
	Transform RotateZ(double angle);

	Transform LookAt(const Point3f& pos, const Point3f& look, const Vector3f& up);
	Transform Orthographic(double near, double far);
	Transform Perspective(double fov, double znear, double zfar);
	Transform Raster(int h, int w);
}

#endif