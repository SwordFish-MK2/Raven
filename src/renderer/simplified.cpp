#include"simplified.h"

namespace Raven {
	void SimplifiedRenderer::render(const Scene& scene) {
		for (int i = 0; i < film.height; ++i) {
			std::cerr << "\rScanlines remaining: " << film.height - 1 - i << ' ' << std::flush;
			for (int j = 0; j < film.width; ++j) {
				Vector3f pixelColor(0.0);
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
						pixelColor += integrate(scene, r, 0);
					}
				}
				double scaler = 1.0 / spp;
				pixelColor *= scaler;
				film.in(pixelColor);
			}
		}
		std::cerr << "\nDone.\n";
		film.write();
	}

	Vector3f SimplifiedRenderer::integrate(const Scene& scene, const Ray& r_in, int depth)const {
		if (depth >= maxDepth)
			return Vector3f(0.0);
		SurfaceInteraction sinter;
		if (scene.intersect(r_in, sinter, epsilon, std::numeric_limits<double>::max())) {
			Vector3f wo = Normalize(r_in.dir);
			Vector3f wi;
			double pdf;
			Vector3f f = sinter.bsdf->sample_f(wo, wi, Point2f(GetRand(), GetRand()), &pdf);
			Ray r_out(sinter.p, wi);
			return 0.5 * integrate(scene, r_out, depth + 1);
		}

		Vector3f unit_direction = Normalize(r_in.dir);
		auto t = 0.5 * (unit_direction[1] + 1.0);
		return (1.0 - t) * Vector3f(1.0, 1.0, 1.0) + t * Vector3f(0.5, 0.7, 1.0);
	}
}