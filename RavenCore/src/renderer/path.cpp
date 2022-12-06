#include<Raven/integrator/path.h>
#include<omp.h>
#include<Raven/core/light.h>
#include<Raven/light/infiniteAreaLight.h>

static omp_lock_t lock;
namespace Raven {
	//��Ⱦ����
	//����Film�е�ÿ�����أ�����������ɫ��ֵ
	void PathTracingIntegrator::render(
		const Scene& scene,
		const Ref<Camera>& camera,
		Ref<Film>& film)const
	{
		int finishedLine = 1;
		double process = 0.0;
		omp_init_lock(&lock);
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
	Spectrum PathTracingIntegrator::integrate(
		const Scene& scene,
		const RayDifferential& rayIn,
		int bounce)const
	{
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
				if (bounce == 0 || specularBounce)
					for (const auto& envlight : scene.infinitAreaLights) {
						Li += beta * envlight->Le(rayIn);
					}
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
				//std::cout <<f<< beta << std::endl;
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

	Ref<PathTracingIntegrator> PathTracingIntegrator::construct(
		const PropertyList& param
	) {
		int spp = param.getInteger("spp", 5);
		int maxDepth = param.getInteger("maxDepth", 10);
		double epsilon = param.getFloat("epsilon", 1e-6);
		return std::make_shared<PathTracingIntegrator>(spp, maxDepth, epsilon);
	}

	//ʵ����ע���ྲ̬����
	PathTracingIntegratorReg PathTracingIntegratorReg::regHelper;

}