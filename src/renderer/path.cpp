#include"path.h"
#include<omp.h>
#include"../core/light.h"

static omp_lock_t lock;
namespace Raven {
	void PathTracingRenderer::render(const Scene& scene) {
		int finishedLine = 1;

		omp_init_lock(&lock);

#pragma omp parallel for
		for (int i = 0; i < film.height; ++i) {
			//		std::cerr << "\rScanlines remaining: " << film.height - 1 - i << ' ' << std::flush;
			double process = (double)finishedLine / film.height;

			omp_set_lock(&lock);
			UpdateProgress(process);
			omp_unset_lock(&lock);

			for (int j = 0; j < film.width; ++j) {
				Spectrum pixelColor(0.0);
				for (int s = 0; s < spp; s++) {
					//camera sample
					auto cu = double(j) + GetRand();
					auto cv = double(i) + GetRand();
					auto fu = GetRand();
					auto fv = GetRand();
					auto t = GetRand();
					CameraSample sample(cu, cv, t, fu, fv);
					Ray r;

					if (camera->GenerateRay(sample, r)) {
						pixelColor += integrate(scene, r);
					}
				}
				double scaler = 1.0 / spp;
				pixelColor *= scaler;

				film.setColor(pixelColor, j, i);
				//film.in(pixelColor);
			}
			finishedLine++;


		}
		omp_destroy_lock(&lock);
		std::cout << "\nDone.\n";
		film.write();
		film.writeTxt();
	}
	//路径追踪算法，暂时只考虑了lambertain
	Spectrum PathTracingRenderer::integrate(const Scene& scene, const Ray& rayIn, int bounce)const {
		//	Spectrum backgroundColor = Spectrum(Spectrum::fromRGB(0.235294, 0.67451, 0.843137));
		Spectrum Li(0.0);
		Spectrum beta(1.0);//光线的衰减参数
		Ray ray = rayIn;

		bool specularBounce = false;
		double etaScale = 1;
		for (; bounce < maxDepth; bounce++) {

			//获取场景与光线的相交信息
			std::optional<SurfaceInteraction> record = scene.intersect(ray, std::numeric_limits<double>::max());
			//光线未与场景相交
			if (!record) {
				break;
			}
			else {
				//光线与场景相交，相交的信息都储存在record中
				Point3f p = record->p;
				Vector3f wo = Normalize(-ray.dir);
				Normal3f n = record->n;

				//只有从相机出发的光线击中光源才直接返回光源的emittion
				if (bounce == 0 || specularBounce) {

					//从相机出发的光线直接击中光源
					if (record->hitLight) {
						Spectrum emittion = record->light->Li(*record, wo);
						Li += beta * emittion;
						return Li;
					}
				}

				for (auto& light : scene.lights) {
					//采样光源,计算以该交点为终点的路径的贡献
					LightSample lightSample;
					Spectrum emit = light->sampleLi(*record, Point2f(GetRand(), GetRand()), &lightSample);
					Spectrum fLight = record->bsdf->f(wo, lightSample.wi);
					double length = (lightSample.p - p).length();
					//double dot1 = Max(0.0, Dot(lightSample.wi, n));
					double dot2 = Max(0.0, Dot(-lightSample.wi, lightSample.n));

					double dot1 = Max(0.0, Dot(lightSample.wi, n));
					//double dot2 = abs(Dot(-lightSample.wi, lightSample.n));
					Spectrum dirLi = emit * fLight * dot1 * dot2 / lightSample.pdf;
					//判断有无遮挡
					//TODO::Debug scene->hit函数及其调用的hit函数，使用hit代替intersect

					Ray shadowRay(p, lightSample.wi);
					if (!scene.hit(shadowRay, length - epsilon))
						Li += dirLi * beta;
				}

				//Vector3f L_dir = SampleAllLights(*record, scene);
				//Li += beta * L_dir;


				//采样brdf，计算出射方向,更新beta
				auto [f, wi, pdf, sampledType] = record->bsdf->sample_f(wo, Point2f(GetRand(), GetRand()));
				if (f == Spectrum(0.0) || pdf == 0.0)
					break;

				//计算衰减
				double cosTheta = abs(Dot(wi, n));
				beta *= f * cosTheta / pdf;
				specularBounce = (sampledType & BxDFType::Specular) != 0;
				ray = record->scartterRay(wi);

				//俄罗斯轮盘赌结束循环
				if (bounce > 3) {
					double q = Max((double).05, 1 - beta.y());
					if (GetRand() < q)
						break;
					beta /= 1 - q;
				}
			}

		}
		return Li;
	}

	//GeometryData PathTracingRenderer::gBuffer(const Ray& ray, const Scene& scene)const {
	//	GeometryData data;
	//	std::optional<SurfaceInteraction> record = scene.intersect(ray, std::numeric_limits<double>::max());
	//	if (record != std::nullopt) {
	//		const Point3f& p = (*record).p;
	//		const Normal3f& n = (*record).n;
	//		data.n = (*record).n;
	//		data.p = (*record).p;
	//		data.hit = true;
	//	}
	//	return data;
	//}

}