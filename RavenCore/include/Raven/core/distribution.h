#ifndef _RAVEN_CORE_DISTRIBUTION_H_
#define _RAVEN_CORE_DISTRIBUTION_H_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include<math.h>
#include<vector>
#include<Raven/core/base.h>
#include<Raven/core/math.h>

//#define UniformSpherePdf 0.25/M_PI
//#define UniformDiskPdf 1/M_PI	
//#define UniformHemiSphere 0.5/M_PI

//edited by FrenchFriseWithPepper

//Brife Description :
//Apart from the Sampler classes,UniformSample uniformly maps the random numbers of [0,1) 
//generated by Sampler to domains where Monte Carlo estimaters will be processed. 

namespace Raven {
	//return a random point in a unit disk
	inline Point2f RejectionSampleDisk() {
		double invertRandMax = 1. / RAND_MAX;
		while (1) {
			double randX = std::rand() * invertRandMax;
			double randY = std::rand() * invertRandMax;
			if ((randX * randX + randY * randY) < 1.0)
				return Point2f(randX, randY);
		}
	}

	//input two random number between [0,1) generated by sampler
	//return a random point in a unit disk generated with respect to given uv parameter
	inline Point2f UniformSampleDisk(const Point2f& uv) {
		double r = sqrt(uv[0]);
		double theta = 2 * M_PI * uv[1];
		return Point2f(r * cos(theta), r * sin(theta));
	}

	inline double UniformDiskPdf() {
		return 1 / M_PI;
	}

	//input two random number between [0,1) generated by sampler
	//return a point on unit sphere 
	inline Point3f UniformSampleSphere(const Point2f& uv) {
		double cosTheta = 1 - 2 * uv[0];
		double phi = 2 * M_PI * uv[1];
		double sinTheta = sqrt(1 - cosTheta * cosTheta);
		double x = sinTheta * cos(phi);
		double y = sinTheta * sin(phi);
		return Point3f(x, y, cosTheta);
	}

	inline double UniformSpherePdf() {
		return 0.25 / M_PI;
	}

	inline Vector3f UniformSampleHemisphere(const Point2f& uv) {
		/*	double phi = 2 * M_PI * uv[0];
			double cosTheta = 1 - uv[1];
			double sinTheta = sqrt(1 - cosTheta * cosTheta);
			double x = sinTheta * cos(phi);
			double y = sinTheta * sin(phi);
			return vectorf3(x, y, cosTheta);*/
		double z = uv[0];
		double r = std::sqrt(std::max((double)0, (double)1. - z * z));
		double phi = 2 * M_PI * uv[1];
		return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
	}

	inline double UniformHemispherePdf() {
		return 0.5 / M_PI;
	}

	inline Vector3f CosWeightedSampleHemisphere(const Point2f& uv) {
		//Point2f sampleOnDisk = UniformSampleDisk(uv);
		//double r2 = pow(sampleOnDisk[0], 2) + pow(sampleOnDisk[1], 2);
		//return Vector3f(sampleOnDisk[0], sampleOnDisk[1], sqrt(1 - r2));
		Point2f d = UniformSampleDisk(uv);
		double z = std::sqrt(Max((double)0, 1 - d.x * d.x - d.y * d.y));
		return Vector3f(d.x, d.y, z);
	}

	inline double CosWeightedHemispherePdf(double cosTheta) {
		return cosTheta / M_PI;
	}

	inline Point3f UniformSampleCone(const Point2f& uv, double cosThetaMax) {
		double cosTheta = 1 - uv[0] + uv[0] * cosThetaMax;
		double phi = 2 * M_PI * uv[1];
		double sinTheta = sqrt(1 - cosTheta * cosTheta);
		double x = sinTheta * cos(phi);
		double y = sinTheta * sin(phi);
		return Point3f(x, y, cosTheta);
	}

	inline double UniformConePdf(double cosThetaMax) {
		return 1 / (2 * M_PI * (1 - cosThetaMax));
	}

	inline Point2f UniformSampleTriangle(const Point2f& uv) {
		double b0 = sqrt(uv[0]);
		double b1 = (1 - b0) * uv[1];
		return Point2f(b0, b1);
	}

	inline double UniformTrianglePdf(double area) {
		return 1.0 / area;
	}

	/// <summary>
	/// ʹ��һ��ȼ��ĳ�ֵ�ֶκ���������1D�ֲ�
	/// </summary>
	struct Distribution1D {
	public:
		//����һ��ֶγ�ֵ����
		Distribution1D(const double* f, int n) :
			function(f, f + n), cdf(n + 1) {
			double invN = 1 / n;
			integral = 0;
			//���趨����Ϊ0-1��ʹ���������ּ��㶨���ֵ�ֵ
			for (int i = 0; i < n; i++)
				integral += function[i] * invN;

			//����CDF��CDFΪPDF�Ļ��֣�PDF���ں�����ĳ������Ļ��ֳ��Ժ����ڶ������ϵĻ���
			cdf[0] = 0;
			if (integral == 0) {
				for (int i = 1; i < n + 1; i++)
					cdf[i] = double(i) / n;
			}
			else {
				double invC = 1 / integral;
				for (int i = 1; i < n + 1; i++) {
					cdf[i] = cdf[i - 1] + function[i-1] * invC * invN;
				}
			}
		}
		int size()const { return function.size(); }
		double funInte()const { return integral; }
		
		//����һ��[0,1]֮�����������ڸ÷ֲ��в���һ��x������pdf
		double sampleContinuous(double u, double* pdf, int* off = nullptr)const {
			//�����淽����u=CDF���ж�uλ�ڷֶκ�������һ��Ƭ��
			int offset = 0;
			for (int i = 0; i < cdf.size(); i++) {
				if (cdf[i] > u)
					break;
				offset = i;
			}
			if (off)*off = offset;

			//����u�ڸ�Ƭ���е�ƫ����
			double du = u - cdf[offset];
			if (cdf[offset + 1] - cdf[offset] > 0)
				du /= (cdf[offset + 1] - cdf[offset]);

			//compute pdf by f(x)/c
			if (pdf) {
				if (integral != 0)
					*pdf = function[offset] / integral;
				else {
					pdf = 0;
				}
			}

			//return x
			return (du + double(offset)) / function.size();
		}

		//����һ��[0,1]֮��������u��ʹ���淽���ӷֶκ����в���u��λ�ڵ��Ƕ�
		int sampleDiscrete(double u, double* pdf, double* remappedU = nullptr)const {

			//find segment surrounding u
			int offset = 0;
			for (int i = 0; i < cdf.size(); i++) {
				if (cdf[i] > u)
					break;
				offset = i;
			}

			//compute pdf
			if (pdf) {
				if (integral != 0)
					*pdf = function[offset] / integral;
				else
					*pdf = 0.0;
			}

			if (remappedU)
				*remappedU = (u - function[offset]) / (function[offset + 1] - function[offset]);
			return offset;
		}

		std::vector<double> function, cdf;//ʹ������洢������CDF
		double integral;//�����ڶ������ڻ��ֵ�ֵ
	};

	/// <summary>
	/// Stratified sampled 2D function and CDF
	/// </summary>
	class Distribution2D {
	public:
		Distribution2D(const double* data, int nu, int nv) {
			conditionalU.resize(nv);
			//����ÿ��Vi,U�����������ܶȺ���Ϊ����U��һά��ֵ�ֶκ���
			for (int i = 0; i < nv; i++)
				conditionalU[i].reset(new Distribution1D(&data[nv * i], nu));

			//V�ı�Ե�ܶȺ���Ϊ����V��һά��ֵ�ֶκ���
			std::vector<double> integralU(nu);
			for (int i = 0; i < nv; i++)
				integralU[i] = conditionalU[i]->funInte();
			marginalV.reset(new Distribution1D(&integralU[0], nv));
		}

		Point2f sampleContinuous(const Point2f& uv, double* pdf)const {
			double pdfv, pdfu;
			//���ȴ�V�ı�Ե�ܶȷֲ��в���V
			int v;
			double px = marginalV->sampleContinuous(uv[1], &pdfv, &v);

			//����V����U�������ܶȷֲ��в���U
			double py = conditionalU[v]->sampleContinuous(uv[0], &pdfu);
			*pdf = pdfv * pdfu;
			return Point2f(px, py);
		}

		double pdf(const Point2f& uv)const {
			int iu = Clamp(int(uv[0] * conditionalU[0]->size()), 0, conditionalU[0]->size() - 1);
			int iv = Clamp(int(uv[1] * marginalV->size()), 0, marginalV->size() - 1);
			return conditionalU[iv]->function[iu] / conditionalU[iv]->funInte();
		}
	private:
		std::vector<std::unique_ptr<Distribution1D>> conditionalU;
		std::unique_ptr<Distribution1D> marginalV;
	};

}

#endif