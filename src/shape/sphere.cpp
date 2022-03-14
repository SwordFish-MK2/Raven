#include"sphere.h"
#include"../core/distribution.h"
namespace Raven {
	bool Sphere::hit(const Ray& r_in, double tmin = 0.001F, double tMax = FLT_MAX)const {
		Ray localRay = (*worldToLocal)(r_in);
		double a = Dot(localRay.dir, localRay.dir);
		double b = 2 * Dot(Vector3f(localRay.origin), localRay.dir);
		double c = Dot(Vector3f(localRay.origin), Vector3f(localRay.origin)) - radius * radius;
		double t0, t1;
		if (!Quadratic(a, b, c, t0, t1))
			return false;
		if (t0 > tMax || t1 < tmin)
			return false;
		return true;

	}

	Bound3f Sphere::worldBound()const {
		Bound3f localBox;
		localBox.pMin = Point3f(-radius, -radius, zMin);
		localBox.pMax = Point3f(radius, radius, zMax);
		return (*localToWorld)(localBox);
	}

	Bound3f Sphere::localBound()const {
		Bound3f localBox;
		localBox.pMin = Point3f(-radius, -radius, zMin);
		localBox.pMax = Point3f(radius, radius, zMax);
		return localBox;
	}

	bool Sphere::intersect(const Ray& r_in, SurfaceInteraction& sinter, double tmin = 0.001F, double tmax = FLT_MAX)const {
		//transform the ray from world space to local space
		Ray localRay = (*worldToLocal)(r_in);

		//perform intersection test
		double a = Dot(localRay.dir, localRay.dir);
		double b = 2.0 * Dot(Vector3f(localRay.origin), localRay.dir);
		double c = Dot(Vector3f(localRay.origin), Vector3f(localRay.origin)) - radius * radius;
		double t0, t1, tHit;
		if (!Quadratic(a, b, c, t0, t1))
			return false;

		//determinate>=0, at least has one hit point,test if tHit is out of time range
		if (t0 > tmax || t1 < tmin)//since t0<t1 by default if t0>tmax || t1< tmin,both values are out of range
			return false;
		//defaultly set tHit = t0, the smaller t value. if t0 is out of time limit, set tHit = t1
		tHit = t0;
		if (t0 < tmin) {
			tHit = t1;
			if (t1 > tmax)
				return false;
		}

		//get pHit and corresponding phi value
		Point3f pHit = localRay.position(tHit);//get hit point 
		double phi = atan(pHit[1] / pHit[0]);
		if (phi < 0)phi += 2 * M_PI;//make sure phi is between 0 and 2PI

		//test the hit point against the clipping parameter(pMax,zMin,zMax
		if (pHit[2] > zMax || pHit[2]<zMin || phi>phiMax) {
			//false
			if (tHit == t0) {
				//pHit0 is clipped, test pHit1
				tHit = t1;
				//get pHit1 and corresponding phi
				pHit = localRay.position(tHit);
				phi = atan(Clamp(pHit[1] / pHit[0], -1.f, 1.f));
				if (phi < 0)phi += 2 * M_PI;

				//test pHit1
				if (pHit[2] > zMax || pHit[2]<zMin || phi>phiMax || t1 > tmax)
					//false,both hit point failed
					return false;
			}
			else
				//t0 is out of time range, pHit1 is clipped
				return false;
		}
		//else true

		//compute geometric parameters of surface intersection
		double u = phi / phiMax;
		double theta = acos(Clamp(pHit[2] / radius, -1.0, 1.0));
		double v = theta - thetaMin / abs(thetaMax - thetaMin);
		Normal3f n = Normal3f(pHit - Point3f(0.0, 0.0, 0.0)).normalized();

		Vector3f dpdu(-pHit[1] * phiMax, -pHit[0] * phiMax, 0);
		Vector3f dpdv = (thetaMax - thetaMin) * Vector3f(pHit[2] * cos(phi), pHit[2] * sin(theta), -radius * sin(theta));

		//coefficients to compute dndu dndv
		double E = abs(Dot(dpdu, dpdu));
		double F = abs(Dot(dpdu, dpdv));
		double G = abs(Dot(dpdv, dpdv));
		Vector3f dpduu = -phiMax * phiMax * Vector3f(pHit[0], pHit[1], 0);
		Vector3f dpduv = (thetaMax - thetaMin) * pHit[2] * phiMax * Vector3f(-sin(phi), cos(phi), 0);
		Vector3f dpdvv = -(thetaMax - thetaMin) * (thetaMax - thetaMin) * Vector3f(pHit[0], pHit[1], pHit[2]);
		double e = Dot(n, dpduu);
		double f = Dot(-n, dpduv);
		double g = Dot(n, dpdvv);
		Vector3f dndu = ((f * F - e * G) / (E * G - F * F)) * dpdu +
			((e * F - f * E) / (E * G - F * F)) * dpdv;
		Vector3f dndv = ((g * F - f * G) / (E * G - F * F)) * dpdu +
			((f * F - g * E) / (E * G - F * F)) * dpdv;

		//generate surfaceIntersection and transform it to world space 
		SurfaceInteraction its;
		its.p = pHit;
		its.t = tHit;
		its.uv = Point2f(u, v);
		its.n = n;
		its.dpdu = dpdu;
		its.dpdv = dpdv;
		its.dndu = dndu;
		its.dndv = dndv;
		sinter = (*this->localToWorld)(its);
		//std::cout << "Ray_Dir:" << r_in.dir << " Normal:" << n << std::endl;
		return true;
	}

	//return sample point coordinate in world space
	SurfaceInteraction Sphere::sample(const Point2f& uv)const {
		//uniformly sample sphere
		Point3f p = UniformSampleSphere(uv);
		p *= radius;
		Normal3f n(p);
		SurfaceInteraction inter;
		inter.p = p;
		inter.n = n;
		inter = (*Shape::localToWorld)(inter);
		return inter;
	}

	//return sample point coordinate in world space
	SurfaceInteraction Sphere::sample(const SurfaceInteraction& inter, const Point2f& uv)const {
		//get point that receving light
		Point3f pCenter = (*localToWorld)(Point3f(0.0));
		double dc = Distance(pCenter, inter.p);
		if (radius > dc) {
			//if pRec is inside the sphere,uniformly sample the sphere
			return sample(uv);
		}
		//compute sample direction
		double cosThetaMax = acos(sqrt(Max(0.0, 1 - pow(radius / dc, 2))));
		double c = 1 / ((1 - cosThetaMax) * 2 * M_PI);
		double cosTheta = 1 - uv[0] * (1 - cosThetaMax);
		double sinTheta = std::sqrt(std::max(0.0, 1 - cosTheta * cosTheta));
		double phi = 2 * M_PI * uv[1];

		//compute sample point on the sphere
		double dtemp = sqrt(radius * radius - dc * dc * sinTheta * sinTheta);
		double ds = dc * cosTheta - dtemp;
		double cosAlpha = (dc * dc - ds * ds + radius * radius) / (2 * dc * radius);
		double sinAlpha = std::sqrt(Max(0.0, 1 - cosAlpha * cosAlpha));
		Point3f pSample(sinAlpha * cos(phi), sinAlpha * sin(phi), cosAlpha);
		SurfaceInteraction lightInter;
		lightInter.p = (*localToWorld)(pSample * radius);
		lightInter.n = (*localToWorld)(Normalize(lightInter.p - pCenter));
		return lightInter;
	}

	double Sphere::pdf(const SurfaceInteraction& inter, const Vector3f& wi)const {
		Point3f pCenter = (*localToWorld)(Point3f(0.0));
		double dc2 = DistanceSquared(pCenter, inter.p);
		if (dc2 < radius * radius)
			//point is inside the sphere, uniform sample sphere
			return pdf(inter);
		//point is outside the shpere, compute cosThetaMax and call UniformConePdf
		double sinThetaMax2 = radius * radius / dc2;
		double cosThetaMax = std::sqrt(1 - sinThetaMax2);
		return UniformConePdf(cosThetaMax);
	}

}