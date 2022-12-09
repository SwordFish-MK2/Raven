#include<Raven/core/integrator.h>
#include<Raven/core/light.h>

namespace Raven {

	Spectrum EvaluateLight(const SurfaceInteraction& record, const Scene& scene, const Light& light) {

		//�����Դ
		for (std::shared_ptr<Light> light : scene.lights) {
			light->preprocess(scene);
		}

		Spectrum L(0.0);

		//�ӹ�Դ�ķֲ��ϲ���
		LightSample lSample;
		Spectrum Le = light.sampleLi(record, Point2f(GetRand(), GetRand()), &lSample);
		if (Le != Spectrum(0.0) && lSample.pdf > 0) {

			//����brdf��scarttingPdf
			Spectrum f = record.bsdf->f(record.wo, lSample.wi) * std::abs(Dot(lSample.wi, record.n));
			double scarttingPdf = record.bsdf->pdf(record.wo, lSample.wi);
			if (f != Spectrum(0.0) && scarttingPdf > 0.0) {

				//�ж�shadow ray�Ƿ��ڵ�
				//Ray shadowRay(record.p, lSample.wi);
				Ray shadowRay = record.scartterRay(lSample.wi);//shadow ray�ӽ��������Դ
				double distance = (record.p - lSample.p).length();
				if (scene.hit(shadowRay, distance - 0.01))
					Le = Spectrum(0.0);	//���shadow ray���ڵ�������Radiance=0

				//����˴β����Ĺ���
				if (Le != Spectrum(0.0)) {
					double weight = PowerHeuristic(1, lSample.pdf, 1, scarttingPdf);
					L += Le * f * weight / lSample.pdf;
				}
			}
		}

		//��brdf�ķֲ��ϲ���

		//����brdf
		auto [f, wi, scarttingPdf, sampledType] =
			record.bsdf->sample_f(record.wo, Point2f(GetRand(), GetRand()));

		double lightPdf = 1.0;
		if (f != Spectrum(0.0) && scarttingPdf > 0) {

			double cosTheta = abs(Dot(wi, record.n));
			f *= cosTheta;

			//����lightPdf����MISȨ��
			lightPdf = light.pdf_Li(record, wi);
			if (lightPdf == 0)
				return L;
			double weight = PowerHeuristic(1, scarttingPdf, 1, lightPdf);

			//����shadow ray �жϹ�Դ�Ƿ��ڵ�
			Ray shadowRay = record.scartterRay(wi);
			std::optional<SurfaceInteraction> intersection = scene.intersect(shadowRay, std::numeric_limits<double>::max());

			//������shadow rayδ���ڵ��������Դ�Ĺ���
			Spectrum Le(0.0);
			if (intersection) {
				if (intersection->hitLight && intersection->light == &light)
					Le += intersection->Le(-wi);
			}
			else
				Le += light.Le(shadowRay);
			if (!Le.isBlack())
				L += weight * f * Le / scarttingPdf;
		}
		return L;
	}

	//��ÿ����Դ�ϲ���һ���㣬����Radiance
	Spectrum SampleAllLights(const SurfaceInteraction& record, const Scene& scene) {
		Spectrum Le(0.0);
		for (auto light : scene.lights) {
			Le += EvaluateLight(record, scene, *light.get());
		}
		return Le;
	}

	Spectrum SampleOneLight(const SurfaceInteraction& record, const Scene& scene, int nSample) {
		Spectrum Le(0.0);
		const std::vector<std::shared_ptr<Light>>& lights = scene.lights;
		Spectrum totalPower(0.0);
		for (auto light : lights) {
			totalPower += light->power();
		}
		Spectrum power;
		double pr = GetRand();
		double p = 1.0;
		Light* chosen;
		for (size_t i = 0; i < lights.size(); i++) {
			power += lights[i]->power();
			p = power.y() / totalPower.y();
			if (p >= pr || i == lights.size() - 1) {
				chosen = lights[i].get();
				break;
			}
		}

		for (int i = 0; i < nSample; i++)
			Le += EvaluateLight(record, scene, *chosen) / p;
		return Le / nSample;
	}
}