#include"shape.h"
#include"interaction.h"

namespace Raven {

	//defaultʵ��Ϊ�ڼ������Ͼ��ȵĲ���һ���㣬�ٽ�pdfת��Ϊ������Ļ���
	std::tuple<SurfaceInteraction, double> Shape::sample(const SurfaceInteraction& ref, const Point2f& rand)const {

		//�ڱ�����ȵĲ���һ���㲢����pdf
		auto [inter, pdf] = sample(rand);

		//��pdf�Ӷ�����Ļ���תΪ������ǵĻ���
		Vector3f wi = inter.p - ref.p;
		double distanceSquared = wi.lengthSquared();
		if (distanceSquared == 0)
			pdf = 0.0;
		else {
			wi = Normalize(wi);
			pdf *= distanceSquared / abs(Dot(inter.n, -wi));
			if (std::isinf(pdf))
				pdf = 0.0;
		}
		return std::tuple<SurfaceInteraction, double>(inter, pdf);
	}

	//defaultʵ��Ϊ���㼸�����Ͼ��Ȳ�����pdf���ٳ����ſɱ�����ʽ
	double Shape::pdf(const SurfaceInteraction& inter, const Vector3f& wi)const {
		//intersect sample ray to light geometry
		Point3f origin = inter.p;
		Ray r = inter.scartterRay(wi);
		HitInfo hitInfo;
		if (!intersect(r, hitInfo, std::numeric_limits<double>::max()))
			return 0;
		SurfaceInteraction lightInter = getGeoInfo(hitInfo.pHit);
		//convert the pdf from integral of light surface to integral of the solid angle of sample point
		double pdf = DistanceSquared(lightInter.p, inter.p) / (std::abs(Dot(lightInter.n, -wi)) * area());
		return pdf;
	}
}