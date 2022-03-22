#ifndef _RAVEN_CORE_BASE_H_
#define _RAVEN_CORE_BASE_H_
#include<iostream>
#include<random>
namespace Raven {
	template<class T>
	class Vector3;
	template<class T>
	class Vector2;
	template<class T>
	class Point3;
	template<class T>
	class Point2;
	template<class T>
	class Normal3;
	template<class T>
	class AABB3;
	template<class T>
	class AABB2;
	template<class T>
	class Texture;
	class Camera;
	class Ray;
	class RayDifferential;
	class Shape;
	class Material;
	class Transform;
	class BSDF;
	class BxDF;
	class Shape;
	class Light;
	class Material;
	class Primitive;
	class Scene;
	class Renderer;
	class VisibilityTester;
	class Triangle;
	class TriangleMesh;
	class Filter;
	struct LightSample;
	template<class T>
	T Max(T t1, T t2) { if (t1 > t2)return t1; return t2; }
	template<class T>
	T Min(T t1, T t2) { if (t1 < t2)return t1; return t2; }
	template<class T>
	T Max(T t0, T t1, T t2) {
		T t3 = t0 > t1 ? t0 : t1;
		return t3 > t2 ? t3 : t2;
	}
	template<class T>
	T Min(T t0, T t1, T t2) {
		T t3 = t0 < t1 ? t0 : t1;
		return t3 < t2 ? t3 : t2;
	}
	inline double Lerp(double t, double x1, double x2) { return (1 - t) * x1 + t * x2; }
	inline double Clamp(double v, double min, double max) {
		if (v > max)
			return max;
		else if (v < min)
			return min;
		return v;
	}
	//solve quadratic function ,return to solutions in tmin and tmax if exist
	inline bool Quadratic(double a, double b, double c, double& tmin, double& tmax) {
		double determinate = b * b - 4 * a * c;
		if (determinate < 0.0) {
			//no solution
			tmin = tmax = -1;
			return false;
		}
		//has one or two solutions, set tmin the smaller one 
		double squareRoot = sqrt(determinate);
		double temp = 0.5 / a;
		tmin = (-b - squareRoot) * temp;
		tmax = (-b + squareRoot) * temp;
		return true;
	}
	template<class T>
	inline void Swap(T& a, T& b) {
		T temp = a;
		a = b;
		b = temp;
	}
	inline bool IsPowerOf2(int32_t x) {
		return x && !(x & (x - 1));
	}
	inline int32_t RoundUpPower2(int32_t x) {
		x--;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x++;
	}
	//use Cramer's law to solve 2x2 linear equation system
	inline bool solve2x2LinearSystem(const double* A, const double* b, double* x0, double* x1) {
		double det = A[0] * A[3] - A[1] * A[2];
		if (det < 1e-8)
			return false;
		//use Cramer's rule to solve linear equation system
		double det0 = b[0] * A[3] - b[1] * A[1];
		double det1 = A[0] * b[1] - A[2] * b[0];
		*x0 = det0 / det;
		*x1 = det1 / det;
		if (std::isnan(*x0) || std::isnan(*x1))
			return false;
		return true;
	}
	//perform gamma correct for sRGB standerd, create a nonlinear relationship between brightness and pixel values
	inline double GammaCorrect(double value) {
		if (value <= 0.0031308)
			return 12.92 * value;
		return 1.055 * pow(value, 1 / 2.4) - 0.055;
	}
	//remove gamma correct and reconstruct a linear relationship between brightness and pixel values
	inline double InverseGammaCorrect(double value) {
		if (value < 0.04045)
			return value / 12.92;
		return pow((value + 0.055) * 1.0 / 1.055, 2.4);
	}
	inline double GetRand() {
		static std::uniform_real_distribution<double> distribution(0.0, 1.0);
		static std::mt19937 generator;
		return distribution(generator);

	}
}
#endif