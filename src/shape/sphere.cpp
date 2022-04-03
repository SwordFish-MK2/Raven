#include"sphere.h"
#include"../core/distribution.h"
namespace Raven {
	bool Sphere::hit(const Ray& r_in, double tMax = FLT_MAX)const {
		//将光线从世界坐标系变换到圆的Local坐标系内
		Ray localRay = (*worldToLocal)(r_in);

		const Point3f& ori = localRay.origin;
		const Vector3f& dir = localRay.dir;

		//求解圆与光线联立的方程
		double a = Dot(dir, dir);
		double b = 2.0 * Dot(Vector3f(ori), dir);
		double c = Dot(Vector3f(ori), Vector3f(ori)) - radius * radius;

		double t0, t1, tHit;
		if (!Quadratic(a, b, c, t0, t1))
			return false;

		//只要两个解有一个位于0到tMax之间，说明光线与圆相交
		if (t0 > 0.0 && t0 < tMax)
			return true;
		else if (t1 > 0.0 && t1 < tMax)
			return true;

		return false;
	}

	Bound3f Sphere::worldBound()const {
		Bound3f localBox;
		//localBox.pMin = Point3f(-radius, -radius, zMin);
		//localBox.pMax = Point3f(radius, radius, zMax);
		return (*localToWorld)(localBox);
	}

	Bound3f Sphere::localBound()const {
		Bound3f localBox;
		//localBox.pMin = Point3f(-radius, -radius, zMin);
		//localBox.pMax = Point3f(radius, radius, zMax);
		return localBox;
	}

	std::optional<SurfaceInteraction> Sphere::intersect(const Ray& r_in, double tMax)const {
		//将光线变换到局部坐标系下
		Ray localRay = (*worldToLocal)(r_in);

		const Point3f& ori = localRay.origin;
		const Vector3f& dir = localRay.dir;

		//perform intersection test
		double a = Dot(dir, dir);
		double b = 2.0 * Dot(Vector3f(ori), dir);
		double c = Dot(Vector3f(ori), Vector3f(ori)) - radius * radius;
		double t0, t1, tHit;
		if (!Quadratic(a, b, c, t0, t1))
			return std::nullopt;

		if (t0 >= tMax || t1 <= 0.0)
			return std::nullopt;
		tHit = t0;
		if (tHit < 0.0) {
			return std::nullopt;
			tHit = t1;
			if (tHit > tMax)
				return std::nullopt;
		}

		Point3f pHit = localRay.position(tHit);

		double phi = -atan(pHit.z / pHit.x);
		if (phi < 0)phi += 2 * M_PI;
		double theta = acos(Clamp(pHit.y / radius, -1, 1));

		double u = Max(0.0, phi / (2 * M_PI));
		double v = theta / M_PI;

		Normal3f nHit = (Normal3f)(pHit - Point3f(0.0)).normalized();

		SurfaceInteraction record;
		record.p = pHit;
		record.t = tHit;
		record.n = nHit;
		record.wo = -localRay.dir;
		record.uv = Point2f(u, v);
		record = (*localToWorld)(record);
		return record;
	

		////compute geometric parameters of surface intersection
		//double u = phi / phiMax;
		//double theta = acos(Clamp(pHit[2] / radius, -1.0, 1.0));
		//double v = theta - thetaMin / abs(thetaMax - thetaMin);
		//Normal3f n = Normal3f(pHit - Point3f(0.0, 0.0, 0.0)).normalized();



		//Vector3f dpdu(-pHit[1] * phiMax, -pHit[0] * phiMax, 0);
		//Vector3f dpdv = (thetaMax - thetaMin) * Vector3f(pHit[2] * cos(phi), 
		//	pHit[2] * sin(theta), -radius * sin(theta));

		////coefficients to compute dndu dndv
		//double E = abs(Dot(dpdu, dpdu));
		//double F = abs(Dot(dpdu, dpdv));
		//double G = abs(Dot(dpdv, dpdv));
		//Vector3f dpduu = -phiMax * phiMax * Vector3f(pHit[0], pHit[1], 0);
		//Vector3f dpduv = (thetaMax - thetaMin) * pHit[2] * phiMax * Vector3f(-sin(phi), cos(phi), 0);
		//Vector3f dpdvv = -(thetaMax - thetaMin) * (thetaMax - thetaMin) * Vector3f(pHit[0], pHit[1], pHit[2]);
		//double e = Dot(n, dpduu);
		//double f = Dot(-n, dpduv);
		//double g = Dot(n, dpdvv);

		//Vector3f dndu = ((f * F - e * G) / (E * G - F * F)) * dpdu +
		//	((e * F - f * E) / (E * G - F * F)) * dpdv;
		//Vector3f dndv = ((g * F - f * G) / (E * G - F * F)) * dpdu +
		//	((f * F - g * E) / (E * G - F * F)) * dpdv;

		////generate surfaceIntersection and transform it to world space 
		//SurfaceInteraction its;
		//its.p = pHit;
		//its.t = tHit;
		//its.uv = Point2f(u, v);
		//its.n = n;
		//its.dpdu = dpdu;
		//its.dpdv = dpdv;
		//its.dndu = dndu;
		//its.dndv = dndv;
		//its.wo = -localRay.dir;
		//its = (*this->localToWorld)(its);


		////return its;

		//return std::nullopt;
	}

	//在圆上均匀采样一个点
	std::tuple<SurfaceInteraction, double> Sphere::sample(const Point2f& uv)const {
		//采样点p与法线n
		Point3f p = UniformSampleSphere(uv);
		Normal3f n(p);

		p *= radius;
		//将n与p变换到世界坐标
		p = (*localToWorld)(p);
		n = (*localToWorld)(n);

		SurfaceInteraction inter;
		inter.p = p;
		inter.n = n;

		//计算pdf
		double pdf = 1 / area();
		return std::tuple<SurfaceInteraction, double>(inter, pdf);
	}

	//基于点p采样圆上的一个点，返回的pdf为对立体角的积分
	std::tuple<SurfaceInteraction, double> Sphere::sample(const SurfaceInteraction& inter, const Point2f& uv)const {
		Point3f pCenter = (*localToWorld)(Point3f(0.0));//圆心的世界坐标

		//若p点在圆内，均匀采样
		double distanceSquared = DistanceSquared(pCenter, inter.p);
		if (distanceSquared < radius * radius) {
			auto [lightInter, pdf] = sample(uv);

			//将采样的pdf转为对立体角的积分
			Vector3f wi = lightInter.p - inter.p;
			double dis2 = wi.lengthSquared();
			if (dis2 == 0.0)
				pdf = 0.0;
			else {
				wi = Normalize(wi);
				pdf *= DistanceSquared(lightInter.p, inter.p) / abs(Dot(lightInter.n, -wi));
			}
			if (std::isinf(pdf))
				pdf = 0.0;
			return std::tuple<SurfaceInteraction, double>(lightInter, pdf);
		}

		//采样p点可见圆锥
		else {
			//计算新的坐标系，使得点p与圆心的连线位于心坐标系的z轴
			Vector3f z = Normalize(pCenter - inter.p);//由参考点为原点，z轴指向圆心
			auto [x, y] = genTBN(z);

			//在以p点与圆的圆锥上采样一个向量
			//计算cosThetaMax
			double dc = Distance(inter.p, pCenter);
			double sinThetaMax = radius / dc;
			double sinThetaMax2 = sinThetaMax * sinThetaMax;
			double cosThetaMax = sqrt(Max(0.0, 1 - sinThetaMax2));
			//采样从P点出射的光线的方向
			double cosTheta = (1 - uv[0]) + uv[0] * cosThetaMax;
			double sinTheta2 = 1 - cosTheta * cosTheta;
			double phi = 2 * uv[1] * M_PI;

			//给定从p点出射的向量，计算该向量与圆的交点pL
			//计算对于pL对于圆心的角度alpha
			double ds = dc * cosTheta - std::sqrt(Max(0.0, radius * radius - dc * sinTheta2));
			double cosAlpha = (dc * dc + radius * radius - ds * ds) /
				(2 * dc * radius);
			double sinAlpha = std::sqrt(Max(0.0, 1 - cosTheta * cosTheta));

			Vector3f nWorld = sinAlpha * cos(phi) * (-x) + sinAlpha * sin(phi) * (-y) + cosAlpha * (-z);
			Point3f  pWorld = pCenter + nWorld * radius;

			SurfaceInteraction lightSample;
			lightSample.p = pWorld;
			lightSample.n = Normal3f(nWorld);

			double pdf = 1 / (2 * M_PI * (1 - cosThetaMax));

			return std::tuple<SurfaceInteraction, double>(lightSample, pdf);
		}
	}

	double Sphere::pdf(const SurfaceInteraction& inter, const Vector3f& wi)const {
		Point3f pCenter = (*localToWorld)(Point3f(0.0));
		double dc2 = DistanceSquared(pCenter, inter.p);

		if (dc2 < radius * radius)
			//点在圆内，均匀采样整个圆并变换pdf为对立体角的积分
			return Shape::pdf(inter, wi);

		//点在圆外，计算cosThetaMax，直接计算pdf
		double sinThetaMax2 = radius * radius / dc2;
		double cosThetaMax = std::sqrt(1 - sinThetaMax2);
		return UniformConePdf(cosThetaMax);
	}

	std::shared_ptr<Sphere> Sphere::build(const Transform* LTW, const Transform* WTL,
		double radius) {
		return std::make_shared<Sphere>(LTW, WTL, radius);
	}
}