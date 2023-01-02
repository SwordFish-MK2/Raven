#include<Raven/shape/sphere.h>
#include<Raven/core/distribution.h>
namespace Raven {

	bool Sphere::hit(const Ray& r_in)const {
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
		if (t0 > 0.0 && t0 < r_in.tMax)
			return true;
		else if (t1 > 0.0 && t1 < r_in.tMax)
			return true;

		return false;
	}

	Bound3f Sphere::worldBound()const {
		Bound3f localBox;
		localBox.pMin = Point3f(-radius, -radius, -radius);
		localBox.pMax = Point3f(radius, radius, radius);
		return (*localToWorld)(localBox);
	}

	Bound3f Sphere::localBound()const {
		Bound3f localBox;
		localBox.pMin = Point3f(-radius, -radius, -radius);
		localBox.pMax = Point3f(radius, radius, radius);
		return localBox;
	}

	bool Sphere::intersect(const Ray& r_in, HitInfo& info)const {
		//将光线变换到局部坐标系下
		Ray localRay = (*worldToLocal)(r_in);

		const Point3f& ori = localRay.origin;
		const Vector3f& dir = localRay.dir;

		//判断光线是否与圆相交
		double a = Dot(dir, dir);
		double b = 2.0 * Dot(Vector3f(ori), dir);
		double c = Dot(Vector3f(ori), Vector3f(ori)) - radius * radius;
		double t0, t1, tHit;
		if (!Quadratic(a, b, c, t0, t1))
			return false;

		if (t0 >= r_in.tMax || t1 <= 0.0)
			return false;
		tHit = t0;
		if (tHit < 0.0) {
			tHit = t1;
			if (tHit > r_in.tMax)
				return false;
		}

		//计算交点信息
		Point3f pHit = localRay.position(tHit);
		pHit *= radius / Distance(pHit, Point3f(0, 0, 0));
		info.setInfo(pHit, -r_in.dir);

		//set tMax
		r_in.tMax = tHit;
		return true;
	}

	SurfaceInteraction Sphere::getGeoInfo(const Point3f& pHit)const {
		double phi = atan2(-pHit.z, pHit.x);
		if (phi < 0)phi += 2 * M_PI;
		double theta = acos(Clamp(pHit.y / radius, -1, 1));

		double u = Max(0.0, phi / (2 * M_PI));
		double v = theta / M_PI;

		Normal3f nHit = (Normal3f)(pHit - Point3f(0.0)).normalized();

		//计算偏导dpdu，dpdv
		double zRadius = std::sqrt(pHit.x * pHit.x + pHit.y * pHit.y);
		double invR = 1. / zRadius;
		double cosPhi = pHit.x * invR;
		double sinPhi = -pHit.z * invR;
		Vector3f dpdu = (2 * M_PI) * Vector3f(pHit.z, 0.0, -pHit.x);
		Vector3f dpdv = M_PI * Vector3f(pHit.y * cosPhi, -sin(theta) * radius, -pHit.y * sinPhi);

		//计算偏导dndu，dndv
		double pi2 = M_PI * M_PI;
		Vector3f d2pduu = -4 * pi2 * Vector3f(pHit.x, 0.0, pHit.z);
		Vector3f d2pduv = -2 * pi2 * pHit.y * Vector3f(sin(phi), 0.0, cos(phi));
		Vector3f d2pdvv = -pi2 * Vector3f(pHit.x, pHit.y, pHit.z);

		double e = Dot(nHit, d2pduu);
		double f = Dot(nHit, d2pduv);
		double g = Dot(nHit, d2pdvv);

		double E = Dot(dpdu, dpdu);
		double F = Dot(dpdu, dpdv);
		double G = Dot(dpdv, dpdv);

		double temp = 1.0 / (E * G - F * F);
		Vector3f dndu = (f * F - e * G) * dpdu * temp +
			(e * F - f * E) * dpdv * temp;
		Vector3f dndv = (g * F - f * G) * dpdu * temp +
			(f * F - g * E) * dpdv * temp;

		//记录相交信息并变换到世界坐标系
		SurfaceInteraction record;
		record.p = pHit;
		record.n = nHit;
		record.uv = Point2f(u, v);
		record.dpdu = dpdu;
		record.dpdv = dpdv;
		record.dndu = dndu;
		record.dndv = dndv;
		record = (*localToWorld)(record);
		return record;

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

	SphereReg SphereReg::regHelper;
}