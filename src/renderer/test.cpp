#include"test.h"
#include<omp.h>

static omp_lock_t lock;
namespace Raven {
	void SimplifiedRenderer::render(const Scene& scene) {
		int finishedLine = 0;

		omp_init_lock(&lock);

#pragma omp parallel
		{
#pragma omp  for
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
							//if (i == film.height / 2 && j == film.width / 2)
							//	std::cout << r.origin << r.dir << "\n";
							pixelColor += integrate(r, scene);
						}
					}
					double scaler = 1.0 / spp;
					pixelColor *= scaler;

					film.setColor(pixelColor, j, i);
					//film.in(pixelColor);
				}
				finishedLine++;

			}
		}
		omp_destroy_lock(&lock);
		std::cout << "\nDone.\n";
		film.write();
		film.writeTxt();
	}

	Spectrum SimplifiedRenderer::integrate(const Ray& r_in, const Scene& scene, int depth)const {
		if (depth >= maxDepth)
			return Spectrum(0.0);
		std::optional<SurfaceInteraction> inter = scene.intersect(r_in, FLT_MAX);
		if (!inter.has_value()) {
			//default color
			auto t = 0.5 * (r_in.dir[1] + 1.0);
			Vector3f c = Lerp(t, Vector3f(1.0f, 1.0f, 1.0f), Vector3f(0.5f, 0.7f, 1.0f));
			return Spectrum::fromRGB(c);
		}

		//incident ray hit scene
		Vector3f wo = Normalize(r_in.dir);		//light out going direction, given by ray class
		//Vector3f wi;							//light incident direction, computed by bxdf
		Normal3f N = Normalize(inter->n);	//shading normal
		Vector3f red(1.0, 0.0, 0.0);
		if (Dot(wo, N) > 0.0)//if light incident from the inner side
			return Spectrum(0.0);

		//pointf2 sample = sampler->get2D();
		Point2f sample(GetRand(), GetRand());
		//double pdf;
		auto [f, wi, pdf, sampledType] = inter->bsdf->sample_f(wo, sample);
		double cosTheta = Clamp(Dot(N, wi), 0.0, 1.0);
		if (Dot(wi, N) < 0)
			return Spectrum(0.0);
		pdf = Clamp(pdf, 0.0001, 1.0);
		Ray r_out(inter->p, wi);
		return  f * integrate(r_out, scene, depth + 1) * cosTheta / pdf;
	}
}
