//#include"normal.h"
//#include"../core/interaction.h"
//
//#include"../core/scene.h"
//
//namespace Raven {
//
//	void NormalRenderer::render(const Scene& scene) {
//		for (int i = 0; i < film.height; ++i) {
//			std::cerr << "\rScanlines remaining: " << film.height - 1 - i << ' ' << std::flush;
//			for (int j = 0; j < film.width; ++j) {
//				Vector3f pixelColor(0.0);
//
//				//camera sample
//				auto cu = double(j) + GetRand();
//				auto cv = double(i) + GetRand();
//				auto fu = GetRand();
//				auto fv = GetRand();
//				auto t = GetRand();
//				CameraSample sample(cu, cv, t, fu, fv);
//				Ray r;
//
//				if (camera->GenerateRay(sample, r)) {
//					pixelColor += integrate(scene, r);
//				}
//
//				film.in(pixelColor);
//			}
//		}
//		std::cerr << "\nDone.\n";
//		film.write();
//		film.writeTxt();
//	}
//
//	Vector3f NormalRenderer::integrate(const Scene& scene, const Ray& r_in, int depth)const {
//
//		std::optional<SurfaceInteraction> record = scene.intersect(r_in, std::numeric_limits<double>::max());
//
//		if (record) {
//			const Normal3f& n = record->n;
//			return 0.5 * Vector3f(n.x + 1, n.y + 1, n.z + 1);
//		}
//		else
//			return Vector3f(0.0,0.0,0.0);
//	}
//
//}