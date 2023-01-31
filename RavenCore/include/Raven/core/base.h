#ifndef _RAVEN_CORE_BASE_H_
#define _RAVEN_CORE_BASE_H_

#include<iostream>
#include<random>
#include<map>
#include<memory>

//#define PATH std::string(RAVEN_PATH)

namespace Raven {
	using Float = double;

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
	template<class T>
	class Mipmap;
	template<class T>
	class Image;
	class Camera;
	class Ray;
	class RayDifferential;
	class Shape;
	class Material;
	class Transform;
	class BSDF;
	class Film;
	class BxDF;
	class Shape;
	class Light;
	class AreaLight;
	class Material;
	class Medium;
	class Primitive;
	class Scene;
	class InfiniteAreaLight;
	class Integrator;
	class VisibilityTester;
	class Triangle;
	class TriangleMesh;
	class Filter;
	class Sampler;
	class RavenObject;
	class RavenClass;
	class PropertyList;
	struct LightSample;
	struct SurfaceInteraction;
	struct Interaction;
	struct MediumInterface;

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

	inline Float Clamp(Float v, Float min, Float max) {
		if (v > max)
			return max;
		else if (v < min)
			return min;
		return v;
	}

	//solve quadratic function ,return to solutions in tmin and tmax if exist
	inline bool Quadratic(double a, double b, double c, double& tmin, double& tmax) {
		double discrim = b * b - 4 * a * c;
		if (discrim <= 0) return false;
		double rootDiscrim = std::sqrt(discrim);
		double q;
		if (b < 0) q = -.5 * (b - rootDiscrim);
		else       q = -.5 * (b + rootDiscrim);
		tmin = q / a;
		tmax = c / q;
		if (tmin > tmax) std::swap(tmin, tmax);
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
		return ++x;
	}
	//use Cramer's law to solve 2x2 linear equation system
	inline bool solve2x2LinearSystem(const double* A, const double* b, double* x0, double* x1) {
		double det = A[0] * A[3] - A[1] * A[2];
		if (std::abs(det) < 1e-8)
			return false;
		//use Cramer's law to solve linear equation system
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

	//��ȡ[0,1]֮��������
	inline double GetRand() {
		static std::uniform_real_distribution<double> distribution(0.0, 1.0);
		static std::mt19937 generator;
		return distribution(generator);

	}
	inline double rgbToFloat(int rgb_value) {
		return rgb_value / 255.0;
	}
	inline double ErfInv(double x) {
		double w, p;
		x = Clamp(x, -.99999f, .99999f);
		w = -std::log((1 - x) * (1 + x));
		if (w < 5) {
			w = w - 2.5f;
			p = 2.81022636e-08f;
			p = 3.43273939e-07f + p * w;
			p = -3.5233877e-06f + p * w;
			p = -4.39150654e-06f + p * w;
			p = 0.00021858087f + p * w;
			p = -0.00125372503f + p * w;
			p = -0.00417768164f + p * w;
			p = 0.246640727f + p * w;
			p = 1.50140941f + p * w;
		}
		else {
			w = std::sqrt(w) - 3;
			p = -0.000200214257f;
			p = 0.000100950558f + p * w;
			p = 0.00134934322f + p * w;
			p = -0.00367342844f + p * w;
			p = 0.00573950773f + p * w;
			p = -0.0076224613f + p * w;
			p = 0.00943887047f + p * w;
			p = 1.00167406f + p * w;
			p = 2.83297682f + p * w;
		}
		return p * x;
	}

	template<class T>
	inline double Log2(T v) {
		return std::log(v) / std::log(2);
	}

	inline void UpdateProgress(double progress)
	{
		int barWidth = 70;

		std::cout << "[";
		int pos = barWidth * (int)progress;
		for (int i = 0; i < barWidth; ++i) {
			if (i < pos) std::cout << "=";
			else if (i == pos) std::cout << ">";
			else std::cout << " ";
		}
		std::cout << "] " << int(progress * 100.0) << " %\r";
		std::cout.flush();
	};

	template<class T>
	class RavenParamSetItem {
		RavenParamSetItem(
			const std::string& name,
			const std::unique_ptr<T[]>& value,
			int nValues = 1) :
			name(name), values(value), nValues(nValues) {}
	private:
		std::string name;
		std::unique_ptr<T[]> values;
		int nValues;
	};

	std::vector<std::string> tokenize(
		const std::string& string,
		const std::string& delim = ",",
		bool includeEmpty = false);


	double toFloat(const std::string& str);

	int toInt(const std::string& str);
}
#endif