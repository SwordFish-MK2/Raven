#include"path.h"
#include<omp.h>
#include"../core/light.h"

static omp_lock_t lock;
namespace Raven {
	void PathTracingRenderer::render(const Scene& scene) {
		int finishedLine = 1;
		double process = 0.0;
		omp_init_lock(&lock);
		omp_set_num_threads(2);
#pragma omp parallel for
		for (int i = 0; i < film->yRes; ++i) {

			//������Ⱦ�Ľ��ȣ����������
			process = (double)finishedLine / film->yRes;
			omp_set_lock(&lock);
			UpdateProgress(process);
			omp_unset_lock(&lock);


			for (int j = 0; j < film->xRes; ++j) {
				Spectrum pixelColor(0.0);
				for (int s = 0; s < spp; s++) {
					//camera sample
					auto cu = double(j) + GetRand();
					auto cv = double(i) + GetRand();
					auto fu = GetRand();
					auto fv = GetRand();
					auto t = GetRand();
					CameraSample sample(cu, cv, t, fu, fv);
					RayDifferential r;

					if (camera->GenerateRayDifferential(sample, r)) {
						pixelColor += integrate(scene, r);
					}
				}
				double scaler = 1.0 / spp;
				pixelColor *= scaler;

				(*film)(j, i) = pixelColor;
			}

			finishedLine++;
		}
		process = (double)finishedLine / film->yRes;
		UpdateProgress(process);
		omp_destroy_lock(&lock);

		film->write();
	}

	//·��׷���㷨
	Spectrum PathTracingRenderer::integrate(const Scene& scene, const RayDifferential& rayIn, int bounce)const {
		//	Spectrum backgroundColor = Spectrum(Spectrum::fromRGB(0.235294, 0.67451, 0.843137));
		Spectrum Li(0.0);
		Spectrum beta(1.0);//���ߵ�˥������
		RayDifferential ray = rayIn;

		bool specularBounce = false;
		double etaScale = 1;
		for (; bounce < maxDepth; bounce++) {

			//��ȡ��������ߵ��ཻ��Ϣ
			std::optional<SurfaceInteraction> record = scene.intersect(ray, std::numeric_limits<double>::max());
			//����δ�볡���ཻ
			if (!record) {
				break;
			}
			else {
				//�����볡���ཻ���ཻ����Ϣ��������record��
				Point3f p = record->p;
				Vector3f wo = Normalize(-ray.dir);
				Normal3f n = record->n;

				//ֻ�д���������Ĺ��߻��й�Դ��ֱ�ӷ��ع�Դ��emittion
				if (bounce == 0 || specularBounce) {

					//����������Ĺ���ֱ�ӻ��й�Դ
					if (record->hitLight) {
						Spectrum emittion = record->light->Li(*record, wo);
						Li += beta * emittion;
						return Li;
					}
				}

				//������Դ
				Spectrum L_dir = SampleAllLights(*record, scene);
				Li += beta * L_dir;

				//����brdf��������䷽��,����beta
				auto [f, wi, pdf, sampledType] = record->bsdf->sample_f(wo, Point2f(GetRand(), GetRand()));
				if (f == Spectrum(0.0) || pdf == 0.0)
					break;

				//����˥��
				double cosTheta = abs(Dot(wi, n));
				beta *= f * cosTheta / pdf;
				specularBounce = (sampledType & BxDFType::Specular) != 0;
				ray = record->scartterRay(wi);

				//����˹���̶Ľ���ѭ��
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

	std::shared_ptr<Renderer>makePathTracingRenderer(
		const std::shared_ptr<Film>& film,
		const std::shared_ptr<Camera>& camera,
		const PropertyList& param) {
		int spp = param.getInteger("spp");
		int maxDepth = param.getInteger("maxDepth");
		double epsilon = param.getFloat("epsilon");
		return std::make_shared<PathTracingRenderer>(camera, film, spp, maxDepth, epsilon);
	}
}