#ifndef _RAVEN_CORE_MATH_H_
#define _RAVEN_CORE_MATH_H_

#include"base.h"
#include<iostream>
namespace Raven {
	using Vector3f = Vector3<double>;
	using Vector3i = Vector3<int>;
	using Vector2f = Vector2<double>;
	using Vector2i = Vector2<int>;
	using Point3f = Point3<double>;
	using Point3i = Point3<int>;
	using Point2f = Point2<double>;
	using Point2i = Point2<int>;
	using Normal3f = Normal3<double>;
	using Normal3i = Normal3<int>;

	/// <summary>
	/// Three dimensional vector class
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class Vector3 {
	public:
		//data------------------------------------------------------------
		T x, y, z;

		//Methods---------------------------------------------------------
		Vector3() :x(0), y(0), z(0) {}
		Vector3(T t) :x(t), y(t), z(t) {}
		Vector3(T xx, T yy, T zz) :x(xx), y(yy), z(zz) {}
		Vector3(const Vector3<T>& v) :x(v.x), y(v.y), z(v.z) {}
		Vector3(const Vector2<T>& v) :x(v.x), y(v.y), z(0) {}
		Vector3(const Point3<T>& p) :x(p.x), y(p.y), z(p.z) {}
		Vector3(const Normal3<T>& n) :x(n.x), y(n.y), z(n.z) {}
		Vector3<T>& operator=(const Vector3<T>& v) {
			x = v.x; y = v.y; z = v.z;
			return *this;
		}
		Vector3<T> operator+(const Vector3<T>& v2) const {
			return Vector3<T>(x + v2.x, y + v2.y, z + v2.z);
		}
		Vector3<T>& operator+=(const Vector3<T>& v2) {
			x += v2.x; y += v2.y; z += v2.z;
			return *this;
		}
		Vector3<T> operator+(const Normal3<T>& n)const {
			return Vector3<T>(x + n.x, y + n.y, z + n.z);
		}
		bool operator==(const Vector3<T>& v)const {
			return (x == v.x && y == v.y && z == v.z);
		}
		bool operator!=(const Vector3<T>& v)const {
			return(x != v.x || y != v.y || z != v.z);
		}
		Vector3<T>& operator+=(const Normal3<T>& n) {
			x += n.x; y += n.y; z += n.z;
		}
		Vector3<T> operator-(const Vector3<T>& v2)const {
			return Vector3<T>(x - v2.x, y - v2.y, z - v2.z);
		}
		Vector3<T>& operator-=(const Vector3<T>& v2) {
			x -= v2.x; y -= v2.y; z -= v2.z;
			return *this;
		}
		T operator[](int i) const {
			//check i>=0&&<3
			if (i == 0)	return x;
			if (i == 1)return y;
			return z;
		}
		T& operator[](int i) {
			if (i == 0)return x;
			if (i == 1)return y;
			return z;
		}
		Vector3<T> operator*(T i) const {
			return Vector3<T>(x * i, y * i, z * i);
		}
		Vector3<T>& operator*=(T i) {
			x *= i; y *= i; z *= i;
			return *this;
		}
		Vector3<T> operator*(const Vector3<T>& v2)const {
			return Vector3<T>(x * v2.x, y * v2.y, z * v2.z);
		}
		Vector3<T>& operator*=(const Vector3<T>& v2) {
			this->x *= v2.x;
			this->y *= v2.y;
			this->z *= v2.z;
			return *this;
		}
		Vector3<T> operator/(double i)const {
			//check i=0
			double k = 1.0f / i;
			return Vector3(x * k, y * k, z * k);
		}
		Vector3<T>& operator/=(double i) {
			//check i=0;
			double k = 1.0f / i;
			x *= k; y *= k; z *= k;
			return *this;
		}
		Vector3<T> operator-()const {
			return Vector3(-x, -y, -z);
		}

		double lengthSquared() const {
			return x * x + y * y + z * z;
		}
		double length() const {
			return sqrt(lengthSquared());
		}
		void normalize() {
			//check length=0
			double k = 1.0f / length();
			x *= k;
			y *= k;
			z *= k;
		}

		Vector3<T> normalized() {
			double k = 1.0f / length();
			return Vector3<T>(x * k, y * k, z * k);
		}
	};

	/// <summary>
	/// Two dimensional vector class
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class Vector2 {
	public:
		//data
		T x, y;
		//Methods--------------------------------------------------
		Vector2() :x(0), y(0) {}
		//exception
		Vector2(T xx, T yy) :x(xx), y(yy) {
			//exceptions not a number
		}
		template<class Type>
		Vector2(const Vector2<T>& v) {
			x = v.x; y = v.y;
		}
		template<class T2>
		Vector2(const Vector3<T>& v) {
			x = v.x; y = v.y;
		}
		//overload operators--------------------------
		Vector2<T>& operator=(const Vector2<T>& v) {
			x = (T)v.x; y = (T)v.y;
			return *this;
		}
		Vector2<T> operator+(const Vector2<T>& v) const {
			return Vector2<T>(x + v.x, y + v.y);
		}
		Vector2<T>& operator+=(const Vector2<T>& v) {
			x += v.x;
			y += v.y;
			return *this;
		}
		Vector2<T> operator-(const Vector2<T>& v)const {
			return Vector2<T>(x - v.x, y - v.y);
		}
		Vector2<T>& operator-=(const Vector2<T>& v) {
			x -= v.x;
			y -= v.y;
			return *this;
		}
		Vector2<T> operator*(T i)const {
			return Vector2<T>(x * i, y * i);
		}
		Vector2<T>& operator*=(T i) {
			x *= i;
			y *= i;
			return *this;
		}
		Vector2<T> operator/(T i)const {
			//CHECK i=0;

			double k = 1.0f / i;
			return Vector2<T>(x * k, y * k);
		}
		Vector2<T>& operator/=(T i) {
			//check i=0;

			double k = 1.0f / i;
			x *= i;
			y *= i;
			return *this;
		}
		T operator[](int i)const {
			//check i>0&&i<2
			if (i == 0)	return  x;
			return y;
		}
		T& operator[](int i) {
			if (i == 0)	return  x;
			return y;
		}
		//negative
		Vector2<T> operator-()const {
			return Vector2(-x, -y);
		}

		//other methods------------------------------
		double lengthSquared() const {
			return x * x + y * y;
		}
		double length() const {
			return sqrt(lengthSquared());
		}
		void normalize() {
			double l = 1.0f / length();
			x *= l;
			y *= l;
		}
	};

	/// <summary>
	/// Three dimensional point class
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class Point3 {
	public:
		T x, y, z;
		//methods
		//constructors-------------------------------------------------		
		Point3() :x(0), y(0), z(0) {}
		Point3(T xx, T yy, T zz) :x(xx), y(yy), z(zz) {
		}
		Point3(const Point3<T>& p) :x(p.x), y(p.y), z(p.z) {}
		explicit Point3(const Vector3<T>& v) { x = v.x; y = v.y; z = v.z; }
		//overload operators-------------------------------------------
		bool operator==(const Point3<T>& t) { return x == t.x && y == t.y && z == t.z; }
		bool operator!=(const Point3<T>& t) { return x != t.x || y != t.y || z != t.z; }

		Point3<T>& operator=(const Point3<T>& p) {
			x = p.x; y = p.y; z = p.z;
			return *this;
		}
		Vector3<T> operator-(const Point3<T>& p) const {
			return Vector3<T>(x - p.x, y - p.y, z - p.z);
		}
		Point3<T> operator-(const Vector3<T>& v) const {
			return Point3<T>(x - v.x, y - v.y, z - v.z);
		}
		Point3<T>& operator-=(const Vector3<T>& v) {
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		Point3<T> operator+(const Vector3<T>& v)const {
			return Point3<T>(x + v.x, y + v.y, z + v.z);
		}
		Point3<T>& operator+=(const Vector3<T>& v) {
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		Point3<T> operator+(const Point3<T>& p)const {
			return Point3<T>(x + p.x, y + p.y, z + p.z);
		}
		template<class U>
		Point3<T> operator*(U t)const {
			return Point3<T>(x * t, y * t, z * t);

		}
		template<class U>
		Point3<T>& operator*=(U t) {
			x *= t;
			y *= t;
			z *= t;
			return *this;
		}
		template<class U>
		Point3<T> operator/(U t)const {
			assert(t != 0);
			double k = 1 / (double)t;
			return Point3<T>(x * k, y * k, z * k);
		}
		template<class U>
		Point3<T>& operator/=(U t) {
			assert(t != 0);
			double k = 1 / (double)t;
			x *= k;
			y *= k;
			z *= k;
			return *this;
		}
		T operator[](int i)const {
			if (i == 0)return x;
			if (i == 1)return y;
			return z;
		}
		T& operator[](int i) {
			if (i == 0)return x;
			if (i == 1)return y;
			return z;
		}
		template<class U>
		explicit operator Vector3<U>() const {
			return Vector3<U>(x, y, z);
		}


	};

	/// <summary>
	/// Two dimensional point class
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class Point2 {
	public:
		T x, y;

		//constructors----------------------------------------
		Point2() { x = 0; y = 0; }
		//exception
		Point2(T xx, T yy) :x(xx), y(yy) {
		}
		Point2(const Point2<T>& p) :x(p.x), y(p.y) {}
		explicit Point2(const Vector2<T>& v) { x = v.x; y = v.y; }
		explicit Point2(const Point3<T>& p) { x = p.x; y = p.y; }
		//overloadd operators---------------------------------
		Point2<T>& operator=(const Point2<T>& p) {
			x = p.x;
			y = p.y;
			return *this;
		}
		Point2<T> operator+(const Vector2<T>& p) const {
			return Point2<T>(x + p.x, y + p.y);
		}
		Point2<T>& operator+=(const Vector2<T>& v) {
			x += v.x;
			y += v.y;
			return *this;
		}
		Point2<T> operator+(const Point2<T>& p)const {
			return Point2<T>(x + p.x, y + p.y);
		}
		Vector2<T> operator-(const Point2<T>& p)const {
			return Vector2<T>(x - p.x, y - p.y);
		}
		Point2<T> operator-(const Vector2<T>& v)const {
			return Point2<T>(x - v.x, y - v.y);
		}
		Point2<T>& operator-=(const Vector2<T>& v) {
			x -= v.x;
			y -= v.y;
			return *this;
		}
		template<class U>
		Point2<T> operator*(U t)const {
			return Point2<T>(x * t, y * t);
		}
		template<class U>
		Point2<T> operator/(U t)const {
			assert(t != 0);
			U k = 1 / t;
			return Point2<T>(x * t, y * t);
		}

		T operator[](int i)const {
			//assert(i >= 0 && i < 2);
			if (i == 0) return x;
			return y;
		}
		T& operator[](int i) {
			if (i == 0)return x;
			return y;
		}
		template<class T2>
		explicit operator Vector2<T2>()const {
			return Vector2<T2>(x, y);
		}
	};

	/// <summary>
	/// Three dimensional vector, this class is used to express normal of object surface
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class Normal3 {
	public:
		T x, y, z;
		Normal3() :x(0.f), y(0.f), z(0.f) {}
		Normal3(T xx, T yy, T zz) :x(xx), y(yy), z(zz) {}
		explicit Normal3(const Vector3<T>& v) { x = v.x; y = v.y; z = v.z; }
		Normal3(const Normal3<T>& n) :x(n.x), y(n.y), z(n.z) {}

		Normal3<T>& operator=(const Vector3<T>& v) {
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		Normal3<T>& operator=(const Normal3<T>& n) {
			x = n.x;
			y = n.y;
			z = n.z;
			return *this;
		}

		Normal3<T> operator+(const Normal3<T>& v2) const {
			return Normal3<T>(x + v2.x, y + v2.y, z + v2.z);
		}

		Normal3<T>& operator+=(const Normal3<T>& v2) {
			x += v2.x; y += v2.y; z += v2.z;
			return *this;
		}

		Normal3<T> operator-(const Normal3<T>& v2)const {
			return Normal3<T>(x - v2.x, y - v2.y, z - v2.z);
		}

		Normal3<T>& operator-=(const Normal3<T>& n)const {
			x -= n.x;
			y -= n.y;
			z -= n.z;
			return *this;
		}

		Normal3<T> operator*(double i) const {
			return Normal3(x * i, y * i, z * i);
		}

		Normal3<T>& operator*=(double i) {
			x *= i; y *= i; z *= i;
			return *this;
		}

		Normal3<T> operator/(double i)const {
			//check i=0
			assert(i != 0);
			double k = 1.0f / i;
			return Normal3(x * k, y * k, z * k);
		}

		Normal3<T>& operator/=(double i) {
			//check i=0;
			assert(i != 0);
			double k = 1.0f / i;
			x *= k; y *= k; z *= k;
			return *this;
		}

		Normal3<T> operator-()const {
			return Normal3(-x, -y, -z);
		}

		T operator[](int i)const {
			//check i>=0&&<3
			if (i == 0)return x;
			if (i == 1)return y;
			return z;
		}

		T& operator[](int i) {
			if (i == 0)return x;
			if (i == 1)return y;
			return z;
		}

		bool operator==(const Normal3<T> n)const {
			return x == n.x && y == n.y && z == n.z;
		}

		bool operator!=(const Normal3<T> n)const {
			return x != n.x || y != n.y || z != n.z;
		}

		template<class T>
		Normal3<T> operator()(T i, const Normal3<T>& n)const {
			return Normal3(n.x * i, n.y * i, n.z * i);
		}

		double length() const {
			return  std::sqrt(x * x + y * y + z * z);
		}

		double lengthSquare()const {
			return x * x + y * y + z * z;
		}

		void normalize() {
			double k = 1.0f / length();
			x *= k;
			y *= k;
			z *= k;
		}

		Normal3<T> normalized() {
			double k = 1.0f / length();
			return Normal3<T>(x *= k, y *= k, z *= k);
		}
	};

	//objects output funtions
	template <class T>
	inline std::ostream& operator<<(std::ostream& os, const Vector3<T>& v) {
		os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
		return os;
	}
	template <class T>
	inline std::ostream& operator<<(std::ostream& os, const Vector2<T>& v) {
		os << "[ " << v.x << ", " << v.y << " ]";
		return os;
	}
	template <class T>
	inline std::ostream& operator<<(std::ostream& os, const Point3<T>& v) {
		os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
		return os;
	}
	template <class T>
	inline std::ostream& operator<<(std::ostream& os, const Point2<T>& v) {
		os << "[ " << v.x << ", " << v.y << " ]";
		return os;
	}
	template<class T>
	inline std::ostream& operator<<(std::ostream& os, const Normal3<T>& n) {
		os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
		return out;
	}

	//Vector related functions

	template<class T>
	Vector3<T> operator*(T t, const Vector3<T>& v) {
		return Vector3<T>(v[0] * t, v[1] * t, v[2] * t);
	}
	template<class T>
	inline Vector3<T> Normalize(const Vector3<T>& v) {
		//check v.length=0
		double k = 1.0f / v.length();
		return Vector3<T>(v.x * k, v.y * k, v.z * k);
	}

	template<class T> inline
		bool SameHemisphere(const Vector3<T>& v, const Vector3<T>& w) {
		return v.z * w.z > 0;
	}

	template<class T> inline
		bool SameHemisphere(const Vector3<T>& v, const Normal3<T>& w) {
		return v.z * w.z > 0;
	}

	template<class T>
	inline int MaxDimention(const Vector3<T>& v) {
		int demention = 0;
		T maxNumber = v[0];
		for (int i = 1; i < 3; i++) {
			if (v[i] > maxNumber) {
				maxNumber = v[i];
				demention = i;
			}
		}
		return demention;
	}

	template<class T>
	inline Vector3<T> Cross(const Vector3<T>& v1, const Vector3<T>& v2) {
		return Vector3<T>(
			v1.y * v2.z - v1.z * v2.y,
			-(v1.x * v2.z - v2.x * v1.z),
			v1.x * v2.y - v2.x * v1.y);
	}
	template<class T>
	inline Vector3<T>Lerp(double t, const Vector3<T>& v1, const Vector3<T>& v2) {
		return Vector3<T>(Lerp(t, v1[0], v2[0]), Lerp(t, v1[1], v2[1]), Lerp(t, v1[2], v2[2]));
	}

	template<class T>
	inline T Dot(const Vector3<T>& v1, const Vector3<T>& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	inline Vector3f SphericalDirection(double sinTheta, double cosTheta, double phi) {
		return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi),
			cosTheta);
	}

	inline Vector3f SphericalDirection(double sinTheta, double cosTheta, double phi,
		const Vector3f& x, const Vector3f& y,
		const Vector3f& z) {
		return sinTheta * std::cos(phi) * x + sinTheta * std::sin(phi) * y +
			cosTheta * z;
	}

	//TODO::Try template<class T> Vector3<T>
//输入一个向量，以该向量为z轴建立一个左手系
	inline std::tuple<Vector3f, Vector3f> genTBN(const Vector3f& v1) {
		Vector3f v2, v3;
		//首先通过将输入向量的一个分量置为零，交换剩下的两个分量并将其中一个分量取负值来求得一个与输入向量垂直的向量v2
		//再通过向量积取得垂直于v1与v2的向量v3
		if (std::abs(v1.x) > std::abs(v1.y))
			v2 = Vector3f(-v1.z, 0, v1.x) /
			std::sqrt(v1.x * v1.x + v1.z * v1.z);
		else
			v2 = Vector3f(0, v1.z, -v1.y) /
			std::sqrt(v1.y * v1.y + v1.z * v1.z);
		v3 = Cross(v1, v2);
		return std::tuple<Vector3f, Vector3f>(v2, v3);
	}

	template<class T>
	inline Vector2<T> Normalize(const Vector2<T>& v) {
		//check v.length=0
		double k = 1.0f / v.length();
		return Vector2<T>(v.x * k, v.y * k);
	}
	template<class T>
	inline T Dot(const Vector2<T>& v1, const Vector2<T>& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}

	//Point related funcitons
	template<class T>
	inline Point3<T> operator*(T t, const Point3<T>& p) {
		return Point3<T>(p.x * t, p.y * t, p.z * t);
	}
	template<class T>
	inline double Distance(const Point3<T>& p1, const Point3<T>& p2) {
		return (p1 - p2).length();
	}
	template<class T>
	inline double DistanceSquared(const Point3<T>& p1, const Point3<T>& p2) {
		return (p1 - p2).lengthSquared();
	}
	template<class T>
	inline Point3<T> Lerp(double t, const Point3<T>& p1, const Point3<T>& p2) {
		return (1 - t) * p1 + t * p2;
	}
	template<class T>
	inline Point3<T> Max(const Point3<T>& p1, const Point3<T>& p2) {
		return Point3<T>(p1.x > p2.x ? p1.x : p2.x,
			p1.y > p2.y ? p1.y : p2.y,
			p1.z > p2.z ? p1.z : p2.z);
	}
	template<class T>
	inline Point3<T> Min(const Point3<T>& p1, const Point3<T>& p2) {
		return Point3<T>(p1.x < p2.x ? p1.x : p2.x,
			p1.y < p2.y ? p1.y : p2.y,
			p1.z < p2.z ? p1.z : p2.z);
	}
	template<class T>
	inline Point3<T> Floor(const Point3<T>& p) {
		return Point3<T>(std::floor(p.x), std::floor(p.y), std::floor(p.z));
	}
	template<class T>
	inline Point3<T> Ceil(const Point3<T>& p) {
		return Point3<T>(std::ceil(p.x), std::ceil(p.y), std::ceil(p.z));
	}
	template<class T>
	inline Point3<T> Abs(const Point3<T>& p) {
		return Point3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z));
	}
	template<class T>
	Point2<T> operator*(T t, const Point2<T>& p) {
		return Point2<T>(p.x * t, p.y * t);
	}
	template<class T>
	inline double Distance(const Point2<T>& p1, const Point2<T>& p2) {
		return (p1 - p2).length();
	}
	template<class T>
	inline double DistanceSquared(const Point2<T>& p1, const Point2<T>& p2) {
		return (p1 - p2).lengthSquared();
	}
	template<class T>
	inline Point2<T> Lerp(double t, const Point2<T>& p1, const Point2<T>& p2) {
		return(1 - t) * p1 + t * p2;
	}
	template<class T>
	inline Point2<T> Max(const Point2<T>& p1, const Point2<T>& p2) {
		return Point2<T>(p1.x > p2.x ? p1.x : p2.x,
			p1.y > p2.y ? p1.y : p2.y);
	}
	template<class T>
	inline Point2<T> Min(const Point2<T>& p1, const Point2<T>& p2) {
		return Point2<T>(p1.x < p2.x ? p1.x : p2.x,
			p1.y < p2.y ? p1.y : p2.y);
	}
	template<class T>
	inline Point2<T> Floor(const Point2<T>& p) {
		return Point2<T>(std::floor(p.x), std::floor(p.y));
	}
	template<class T>
	inline Point2<T> Ceil(const Point2<T>& p) {
		return Point2<T>(std::ceil(p.x), std::ceil(p.y));
	}
	template<class T>
	inline Point2<T> Abs(const Point2<T>& p) {
		return Point2<T>(std::abs(p.x), std::abs(p.y));
	}

	//Normal related functions
	template<class T>
	inline Normal3<T> operator*(T t, Normal3<T> n) {
		return Normal3<T>(n.x * t, n.y * t, n.z * t);
	}
	template<class T>
	inline Normal3<T> Normalize(const Normal3<T>& n) {
		T k = 1.0f / n.length();
		return Normal3<T>(n.x * k, n.y * k, n.z * k);
	}
	template<class T> inline
		T Dot(const Normal3<T>& n1, const Normal3<T>& n2) {
		return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
	}
	template<class T> inline
		T Dot(const Normal3<T>& n, const Vector3<T>& v) {
		return n.x * v.x + n.y * v.y + n.z * v.z;
	}
	template<class T> inline
		T Dot(const Vector3<T>& v, const Normal3<T>& n) {
		return n.x * v.x + n.y * v.y + n.z * v.z;
	}

	template<class T> inline
		bool SameHemisphere(const Normal3<T>& n, const Normal3<T>& w) {
		return n.z * w.z > 0;
	}

	//filp the normal so that it lies in the same hemispere with a vector
	template<class T> inline
		Normal3<T> FaceForward(const Normal3<T>& n, const Vector3<T>& v) {
		return (Dot(n, v) < 0.f) ? -n : n;
	}
	template<class T> inline
		Normal3<T> FaceForward(const Normal3<T>& n, const Normal3<T>& v) {
		return (Dot(n, v) < 0.f) ? -n : n;
	}
}

#endif