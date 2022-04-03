#include"bxdf.h"

namespace Raven {

	double FDielectric(double cosI, double etaI, double etaT) {
		//cosThetaI = Clamp(cosThetaI, -1.0, 1.0);

		////如果cosTheta小于0，则说明此时光线为出射，因此交换etaI与etaT
		//if (cosThetaI < 0.f) {
		//	std::swap(etaI, etaT);
		//	cosThetaI = abs(cosThetaI);
		//}

		//double sinThetaI = sqrt(Max(0.0, 1.0 - cosThetaI * cosThetaI));
		//double sinThetaT = sinThetaI * etaI / etaT;
		//double cosThetaT = sqrt(Max(0.0, 1.0 - sinThetaT * sinThetaT));

		////全反射
		//if (sinThetaT >= 1) return 1;

		//double frePara = (etaT * cosThetaI - etaI * cosThetaT) /
		//	(etaT * cosThetaI + etaI * cosThetaT);
		//double frePerp = (etaI * cosThetaI - etaT * cosThetaT) /
		//	(etaI * cosThetaI + etaT * cosThetaT);

		//return 0.5 * (frePara * frePara + frePerp * frePerp);

		if (cosI <= 0)
		{
			std::swap(etaI, etaT);
		}
		// Compute sini using Snell's law
		float sinT = etaI / etaT * sqrt(Max(0.0, 1 - cosI * cosI));
		// Total internal reflection
		if (sinT >= 1)
		{
			return 1.0;
		}
		else
		{
			float cosT = sqrt(Max(0.0, 1.0 - sinT * sinT));
			cosI = fabs(cosI);
			float Rs = ((etaT * cosI) - (etaI * cosT)) /
				((etaT * cosI) + (etaI * cosT));
			float Rp = ((etaI * cosI) - (etaT * cosT)) /
				((etaI * cosI) + (etaT * cosT));
			return  (Rs * Rs + Rp * Rp) / 2;
		}
		// As a consequence of the conservation of energy, transmittance is given by:
		// kt = 1 - kr;

	}

	double FConductor(double cosTheta, double etaI, double etaT, double k) {
		cosTheta = Clamp(cosTheta, 0.001, 1.0);
		double eta = etaT / etaI;
		double etaK = k / etaI;

		double CosTheta2 = cosTheta * cosTheta;
		double SinTheta2 = 1.f - CosTheta2;
		double Eta2 = eta * eta;
		double Etak2 = etaK * etaK;

		double t0 = Eta2 - Etak2 - SinTheta2;//>0
		double a2plusb2 = sqrt(t0 * t0 + 4 * Eta2 * Etak2);//>0
		double t1 = a2plusb2 + CosTheta2;//>0
		double a = sqrt(0.5f * (a2plusb2 + t0));
		double t2 = 2 * a * cosTheta;//>0
		double Rs = (t1 - t2) / (t1 + t2);

		double t3 = CosTheta2 * a2plusb2 + SinTheta2 * SinTheta2;
		double t4 = t2 * SinTheta2;
		double Rp = Rs * (t3 - t4) / (t3 + t4);

		double value = 0.5 * (Rs + Rp);
		if (value > 1.0)
			std::cout << value << std::endl;
		return value;

	}
}