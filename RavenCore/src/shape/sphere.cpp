#include<Raven/shape/sphere.h>
#include<Raven/core/distribution.h>
namespace Raven {

	bool Sphere::hit(const Ray& r_in)const {
		//�����ߴ���������ϵ�任��Բ��Local����ϵ��
		Ray localRay = (*worldToLocal)(r_in);

		const Point3f& ori = localRay.origin;
		const Vector3f& dir = localRay.dir;

		//���Բ����������ķ���
		double a = Dot(dir, dir);
		double b = 2.0 * Dot(Vector3f(ori), dir);
		double c = Dot(Vector3f(ori), Vector3f(ori)) - radius * radius;

		double t0, t1, tHit;
		if (!Quadratic(a, b, c, t0, t1))
			return false;

		//ֻҪ��������һ��λ��0��tMax֮�䣬˵��������Բ�ཻ
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
		//�����߱任���ֲ�����ϵ��
		Ray localRay = (*worldToLocal)(r_in);

		const Point3f& ori = localRay.origin;
		const Vector3f& dir = localRay.dir;

		//�жϹ����Ƿ���Բ�ཻ
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

		//���㽻����Ϣ
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

		//����ƫ��dpdu��dpdv
		double zRadius = std::sqrt(pHit.x * pHit.x + pHit.y * pHit.y);
		double invR = 1. / zRadius;
		double cosPhi = pHit.x * invR;
		double sinPhi = -pHit.z * invR;
		Vector3f dpdu = (2 * M_PI) * Vector3f(pHit.z, 0.0, -pHit.x);
		Vector3f dpdv = M_PI * Vector3f(pHit.y * cosPhi, -sin(theta) * radius, -pHit.y * sinPhi);

		//����ƫ��dndu��dndv
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

		//��¼�ཻ��Ϣ���任����������ϵ
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

	//��Բ�Ͼ��Ȳ���һ����
	std::tuple<SurfaceInteraction, double> Sphere::sample(const Point2f& uv)const {
		//������p�뷨��n
		Point3f p = UniformSampleSphere(uv);
		Normal3f n(p);

		p *= radius;
		//��n��p�任����������
		p = (*localToWorld)(p);
		n = (*localToWorld)(n);

		SurfaceInteraction inter;
		inter.p = p;
		inter.n = n;

		//����pdf
		double pdf = 1 / area();
		return std::tuple<SurfaceInteraction, double>(inter, pdf);
	}

	//���ڵ�p����Բ�ϵ�һ���㣬���ص�pdfΪ������ǵĻ���
	std::tuple<SurfaceInteraction, double> Sphere::sample(const SurfaceInteraction& inter, const Point2f& uv)const {
		Point3f pCenter = (*localToWorld)(Point3f(0.0));//Բ�ĵ���������

		//��p����Բ�ڣ����Ȳ���
		double distanceSquared = DistanceSquared(pCenter, inter.p);
		if (distanceSquared < radius * radius) {
			auto [lightInter, pdf] = sample(uv);

			//��������pdfתΪ������ǵĻ���
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

		//����p��ɼ�Բ׶
		else {
			//�����µ�����ϵ��ʹ�õ�p��Բ�ĵ�����λ��������ϵ��z��
			Vector3f z = Normalize(pCenter - inter.p);//�ɲο���Ϊԭ�㣬z��ָ��Բ��
			auto [x, y] = genTBN(z);

			//����p����Բ��Բ׶�ϲ���һ������
			//����cosThetaMax
			double dc = Distance(inter.p, pCenter);
			double sinThetaMax = radius / dc;
			double sinThetaMax2 = sinThetaMax * sinThetaMax;
			double cosThetaMax = sqrt(Max(0.0, 1 - sinThetaMax2));
			//������P�����Ĺ��ߵķ���
			double cosTheta = (1 - uv[0]) + uv[0] * cosThetaMax;
			double sinTheta2 = 1 - cosTheta * cosTheta;
			double phi = 2 * uv[1] * M_PI;

			//������p�����������������������Բ�Ľ���pL
			//�������pL����Բ�ĵĽǶ�alpha
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
			//����Բ�ڣ����Ȳ�������Բ���任pdfΪ������ǵĻ���
			return Shape::pdf(inter, wi);

		//����Բ�⣬����cosThetaMax��ֱ�Ӽ���pdf
		double sinThetaMax2 = radius * radius / dc2;
		double cosThetaMax = std::sqrt(1 - sinThetaMax2);
		return UniformConePdf(cosThetaMax);
	}

	SphereReg SphereReg::regHelper;
}