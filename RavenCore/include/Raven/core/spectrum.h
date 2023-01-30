
#ifndef _RAVEN_CORE_SPECTRUM_H_
#define _RAVEN_CORE_SPECTRUM_H_

#include<Raven/core/base.h>
#include<Raven/core/math.h>
#include <limits>
#include<tuple>

namespace Raven {
	static const int nSamples = 60;
	static const int sampledLambdaStart = 400;
	static const int sampledLambdaEnd = 700;

	template<int nSpectrumSamples>
	class CoefficientSpectrum {
	public:
		double c[nSpectrumSamples];
		static const int sampleNumber = nSpectrumSamples;
		CoefficientSpectrum() {
			for (int i = 0; i < sampleNumber; i++) {
				c[i] = 0.f;
			}
		}
		CoefficientSpectrum(double v) {
			for (int i = 0; i < sampleNumber; i++)
				c[i] = v;
		}
		CoefficientSpectrum(const CoefficientSpectrum& spectrum) {
			for (int i = 0; i < sampleNumber; i++)
				c[i] = spectrum.c[i];
		}
		CoefficientSpectrum operator+(const CoefficientSpectrum& s)const {
			CoefficientSpectrum temp;
			for (int i = 0; i < sampleNumber; i++)
				temp.c[i] = this->c[i] + s.c[i];
			return temp;
		}
		CoefficientSpectrum& operator+=(const CoefficientSpectrum& s) {
			for (int i = 0; i < sampleNumber; i++)
				c[i] += s.c[i];
			return *this;
		}
		CoefficientSpectrum operator-(const CoefficientSpectrum& s)const {
			CoefficientSpectrum temp;
			for (int i = 0; i < sampleNumber; i++)
				temp.c[i] = this->c[i] - s.c[i];
			return temp;
		}
		CoefficientSpectrum operator-()const {
			CoefficientSpectrum temp = *this;
			for (int i = 0; i < sampleNumber; i++)
				temp.c[i] *= -1;
			return temp;
		}
		CoefficientSpectrum& operator-=(const CoefficientSpectrum& s) {
			for (int i = 0; i < sampleNumber; i++)
				c[i] -= s.c[i];
			return *this;
		}
		CoefficientSpectrum operator*(const CoefficientSpectrum& s)const {
			CoefficientSpectrum temp = *this;
			for (int i = 0; i < sampleNumber; i++)
				temp.c[i] *= s.c[i];
			return temp;
		}
		CoefficientSpectrum& operator*=(const CoefficientSpectrum& s) {
			for (int i = 0; i < sampleNumber; i++) {
				c[i] *= s.c[i];
			}
			return *this;
		}
		CoefficientSpectrum operator*(double t)const {
			CoefficientSpectrum temp;
			for (int i = 0; i < sampleNumber; i++) {
				temp.c[i] = this->c[i] * t;
			}
			return temp;
		}
		CoefficientSpectrum& operator*=(double t) {
			for (int i = 0; i < sampleNumber; i++)
				c[i] *= t;
			return *this;
		}

		template<int n>
		friend inline CoefficientSpectrum operator*(double a, const CoefficientSpectrum& s);

		CoefficientSpectrum operator/(const CoefficientSpectrum& s)const {
			CoefficientSpectrum temp = *this;
			for (int i = 0; i < sampleNumber; i++)
				temp.c[i] /= s.c[i];
			return temp;
		}

		CoefficientSpectrum& operator/=(const CoefficientSpectrum& s) {
			for (int i = 0; i < sampleNumber; i++)
				c[i] /= s.c[i];
			return *this;
		}

		CoefficientSpectrum operator/(double t)const {
			CoefficientSpectrum temp;
			for (int i = 0; i < sampleNumber; i++)
				temp.c[i] = c[i] / t;
			return temp;
		}

		CoefficientSpectrum& operator/=(double t) {
			for (int i = 0; i < sampleNumber; i++)
				c[i] /= t;
			return *this;
		}

		CoefficientSpectrum& operator=(const CoefficientSpectrum& s) {
			for (int i = 0; i < sampleNumber; i++)
				c[i] = s.c[i];
			return *this;
		}

		bool operator==(const CoefficientSpectrum& s)const {
			for (int i = 0; i < sampleNumber; i++) {
				if (c[i] != s.c[i])
					return false;
			}
			return true;
		}

		bool operator!=(const CoefficientSpectrum& s)const {
			for (int i = 0; i < sampleNumber; i++) {
				if (c[i] != s.c[i])
					return true;
			}
			return false;
		}

		double& operator[](int i) {
			if (i < sampleNumber) {
				return c[i];
			}
		}

		double operator[](int i) const {
			return c[i];
		}

		CoefficientSpectrum Clamp(double min = 0.F, double max = std::numeric_limits<Float>::max())const {
			CoefficientSpectrum temp;
			for (int i = 0; i < sampleNumber; i++) {
				temp.c[i] = Clamp(c[i], min, max);
			}
			return temp;
		}

		template<int n>
		friend inline CoefficientSpectrum Pow(const CoefficientSpectrum& s, double p);

		template<int n>
		friend inline CoefficientSpectrum Sqrt(const CoefficientSpectrum& s);

		template<int n>
		friend inline CoefficientSpectrum Exp(const CoefficientSpectrum& s);

		bool hasNaNs() {
			for (int i = 0; i < sampleNumber; i++)
				if (std::isnan(c[i]))
					return true;
			return false;
		}

		bool isBlack() {
			for (int i = 0; i < sampleNumber; i++) {
				if (c[i] != 0.f)
					return false;
			}
			return true;
		}

		friend inline std::ostream& operator<<(std::ostream& out, const CoefficientSpectrum& s) {
			out << "[";
			for (int i = 0; i < sampleNumber; i++) {
				out << s.c[i];
				if (i < sampleNumber - 1)
					out << ",";
			}
			out << "]";
			return out;
		}
	};

	template<int n>
	CoefficientSpectrum<n> Clamp(const CoefficientSpectrum <n>& s, double min, double max) {
		CoefficientSpectrum<n> temp;
		for (int i = 0; i < n; i++) {
			temp[i] = Clamp(s[i], min, max);
		}
		return temp;
	}
	template<int n>
	CoefficientSpectrum<n> Lerp(double t, const CoefficientSpectrum<n>& s1, const CoefficientSpectrum<n> s2) {
		CoefficientSpectrum<n> temp;
		for (int i = 0; i < n; i++) {
			temp[i] = Lerp(t, s1[i], s2[i]);
		}
		return temp;
	}


	static const int nXYZSamples = 471;
	extern const double CIEX[nXYZSamples];
	extern const double CIEY[nXYZSamples];
	extern const double CIEZ[nXYZSamples];
	extern const double CIELambda[nXYZSamples];
	static const double CIE_Y_integral = 106.856895;


	static const int nRGBSamples = 32;
	extern const double RGBLambda[nRGBSamples];
	extern const double RGBReflRed[nRGBSamples];//original colors
	extern const double RGBReflGreen[nRGBSamples];
	extern const double RGBReflBlue[nRGBSamples];
	extern const double RGBReflWhite[nRGBSamples];//red+green+blue
	extern const double RGBReflCyan[nRGBSamples];//green+blue
	extern const double RGBReflMagenta[nRGBSamples];//red+green
	extern const double RGBReflYellow[nRGBSamples];//red+blue

	extern const double RGBIllumRed[nRGBSamples];
	extern const double RGBIllumGreen[nRGBSamples];
	extern const double RGBIllumBlue[nRGBSamples];
	extern const double RGBIllumWhite[nRGBSamples];
	extern const double RGBIllumCyan[nRGBSamples];
	extern const double RBGIllumMagenta[nRGBSamples];
	extern const double RGBIllumYellow[nRGBSamples];

	enum RGBType {
		RGBReflection, RGBIllumination
	};

	class SampledSpectrum :public CoefficientSpectrum<nSamples> {
	private:
		//static curves
		static SampledSpectrum XCurve;
		static SampledSpectrum YCurve;
		static SampledSpectrum ZCurve;

		static SampledSpectrum ReflRedCurve;
		static SampledSpectrum ReflGreenCurve;
		static SampledSpectrum ReflBlueCurve;
		static SampledSpectrum ReflWhiteCurve;
		static SampledSpectrum ReflCyanCurve;
		static SampledSpectrum ReflMagentaCurve;
		static SampledSpectrum ReflYellowCurve;

		static SampledSpectrum IllumRedCurve;
		static SampledSpectrum IllumGreenCurve;
		static SampledSpectrum IllumBlueCurve;
		static SampledSpectrum IllumWhiteCurve;
		static SampledSpectrum IllumCyanCurve;
		static SampledSpectrum IllumMagentaCurve;
		static SampledSpectrum IllumYellowCurve;
	public:
		//static methods
		static void init();//compute all static curves
		static SampledSpectrum fromRGB(const double rgb[3], RGBType type = RGBReflection);
		static SampledSpectrum fromXYZ(const double xyz[3], RGBType type = RGBReflection);
		static SampledSpectrum fromSampled(const double* lambda, const double* v, int n);

		SampledSpectrum() :CoefficientSpectrum(0.f) {}
		SampledSpectrum(double v) :CoefficientSpectrum(v) {}
		void toXYZ(double xyz[3])const;//compute xyz values from a arbitrary sampledSpectrum
		void toRGB(double rgb[3])const;//compute xyz values from SPD and then compute xyz values to rgb values
		double y()const;//compute y value from a arbitrary sampledSpectrum
	};

	//quick sort 
	void Sort(double* lamda, double* v, int start, int end);
	//compute avertage spectrum values in sample range
	double AverageSpectrumSample(const double* lamda, const double* v, int n, int lamdaStart, int lamdaEnd);
	//test if given sample array is sorted
	bool SampleSorted(const double* lamda, const double* v, int n);
	//sort sample array with quick sort method
	void SortSample(double* lamda, double* v, int n);

	//convertion to RGB spectrum based on standard set defined on high-definition teltvision
	inline void XYZToRGB(const double xyz[3], double rgb[3]) {
		//precomputed matrix of transform xyz values to rgb values from pbrt
		rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
		rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
		rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
	}
	inline void RGBToXYZ(const double rgb[3], double xyz[3]) {
		//invert matrix
		xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
		xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
		xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
	}

	class RGBSpectrum :public CoefficientSpectrum<3> {
	public:
		RGBSpectrum() :CoefficientSpectrum(0.f) {}
		RGBSpectrum(double r, double g, double b) {
			c[0] = r;
			c[1] = g;
			c[2] = b;
		}
		RGBSpectrum(const CoefficientSpectrum<3>& s) :CoefficientSpectrum(s) {}
		RGBSpectrum(const RGBSpectrum& s, RGBType type = RGBReflection) {
			*this = s;
		}
		static RGBSpectrum fromRGBInt(int v0, int v1, int v2);
		static RGBSpectrum fromSampled(const double* lambda, const double* v, int n);
		static RGBSpectrum fromXYZ(const double xyz[3], RGBType type = RGBReflection);
		static RGBSpectrum fromRGB(const double rgb[3], RGBType type = RGBReflection);
		static RGBSpectrum fromRGB(double r, double g, double b, RGBType type = RGBReflection);
		static RGBSpectrum fromRGB(const Vector3f& v, RGBType type = RGBReflection);
		const RGBSpectrum& toRGBSpectrum()const {
			return *this;
		}
		double y()const;
		void toRGB(double* rgb)const;
		void toXYZ(double* xyz)const;
	private:
		static double interpolateSpectrumSamples(const double* lambda, const double* values, int n, int l);
	};

	//void getSpectrumSample(std::shared_ptr<double[]> lambda, std::shared_ptr<double[]> v, int* n, const std::string& str);

	using Spectrum = RGBSpectrum;

	template<int n>
	inline CoefficientSpectrum<n> operator*(double a, const CoefficientSpectrum<n>& s) {
		CoefficientSpectrum<n> temp;
		for (int i = 0; i < n; i++) {
			temp.c[i] = s.c[i] * a;
		}
		return temp;
	}

	template<int n>
	inline CoefficientSpectrum<n> Pow(const CoefficientSpectrum<n>& s, double p) {
		CoefficientSpectrum<n> temp;
		for (int i = 0; i < n; i++) {
			temp.c[i] = std::pow(s.c[i], p);
		}
		return temp;
	}

	template<int n>
	inline CoefficientSpectrum<n> Sqrt(const CoefficientSpectrum<n>& s) {
		CoefficientSpectrum<n> temp;
		for (int i = 0; i < n; i++) {
			temp.c[i] = std::sqrt(s.c[i]);
		}
		return temp;
	}

	template<int n>
	inline CoefficientSpectrum<n> Exp(const CoefficientSpectrum<n>& s) {
		CoefficientSpectrum<n> temp;
		for (int i = 0; i < n; i++) {
			temp.c[i] = std::exp(s.c[i]);
		}
		return temp;
	}

}

#endif