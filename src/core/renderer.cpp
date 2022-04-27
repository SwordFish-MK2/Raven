#include"renderer.h"
#include"light.h"

namespace Raven {

	Spectrum EvaluateLight(const SurfaceInteraction& record, const Scene& scene, const Light& light) {

		Spectrum L(0.0);

		//从光源的分布上采样
		LightSample lSample;
		Spectrum Le = light.sampleLi(record, Point2f(GetRand(), GetRand()), &lSample);
		if (Le != Spectrum(0.0) && lSample.pdf > 0) {

			//计算brdf与scarttingPdf
			Spectrum f = record.bsdf->f(record.wo, lSample.wi) * std::abs(Dot(lSample.wi, record.n));
			double scarttingPdf = record.bsdf->pdf(record.wo, lSample.wi);
			if (f != Spectrum(0.0) && scarttingPdf > 0.0) {

				//判断shadow ray是否被遮挡
				//Ray shadowRay(record.p, lSample.wi);
				Ray shadowRay = record.scartterRay(lSample.wi);
				double distance = (record.p - lSample.p).length();
				if (scene.hit(shadowRay, distance - 0.01))
					Le = Spectrum(0.0);	//如果shadow ray被遮挡，返回Radiance=0

					//计算此次采样的贡献
				if (Le != Spectrum(0.0)) {
					double weight = PowerHeuristic(1, lSample.pdf, 1, scarttingPdf);
					L += Le * f * weight / lSample.pdf;
				}
			}
		}

		//从brdf的分布上采样

		//采样brdf
		auto [f,wi,scarttingPdf,sampledType] = 
			record.bsdf->sample_f(record.wo, Point2f(GetRand(), GetRand()));

		double lightPdf = 1.0;
		if (f != Spectrum(0.0) && scarttingPdf > 0) {

			double cosTheta = abs(Dot(wi, record.n));
			f *= cosTheta;

			//计算lightPdf，求MIS权重
			lightPdf = light.pdf_Li(record, wi);
			if (lightPdf == 0)
				return L;
			double weight = PowerHeuristic(1, scarttingPdf, 1, lightPdf);

			//出射shadow ray 判断光源是否被遮挡
			Ray shadowRay = record.scartterRay(wi);
			std::optional<SurfaceInteraction> intersection = scene.intersect(shadowRay, std::numeric_limits<double>::max());

			//采样的shadow ray未被遮挡，计算光源的贡献
			if (intersection && (*intersection).hitLight && (*intersection).light == &light) {
				Spectrum Le = light.Li(*intersection, -wi);
				if (Le != Spectrum(0.0))
					L += weight * f * Le / scarttingPdf;
			}
		}
		return L;
	}

	//在每个光源上采样一个点，计算Radiance
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