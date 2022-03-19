#include"simplified.h"
#include"../core/light.h"
namespace Raven {
	void SimplifiedRenderer::render(const Scene& scene) {
		for (int i = 0; i < film.height; ++i) {
			std::cerr << "\rScanlines remaining: " << film.height - 1 - i << ' ' << std::flush;
			for (int j = 0; j < film.width; ++j) {
				Vector3f pixelColor(0.0);
				//auto cu = double(j);
				//auto cv = double(i);
				//CameraSample cs(cu, cv, 0.0, 0.0, 0.0);
				//Ray gr;
				//if (camera->GenerateRay(cs, gr)) {
				//	film.setGBuffer(gBuffer(gr,scene),j,i);
				//}
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
//		film.writeColor();
	}

	Vector3f SimplifiedRenderer::integrate(const Scene& scene, const Ray& r_in, int depth)const {
		if (depth >= maxDepth)
			return Vector3f(0.0);
		SurfaceInteraction sinter;
		if (scene.intersect(r_in, sinter, epsilon, std::numeric_limits<double>::max())) {
			//get shading data
			Vector3f L_dir(0.0);
			Vector3f L_ind(0.0);
			Vector3f wo = Normalize(-r_in.dir);
			Vector3f wi;
			Vector3f n = Normalize(Vector3f(sinter.n));
			const Point3f& p = sinter.p;

			//光线入射Area Light
			if (sinter.hitLihgt) {
				return sinter.light->Li(sinter, wo);
			}

			//采样光源，计算直接光照
			double rand = GetRand();
			const Light* light = scene.chooseLight(rand);
			Point2f randuv(GetRand(), GetRand());
			SurfaceInteraction lightSample;
			Vector3f wiLight;//从交点指向光源
			double pdfLight = 0.0;

			for (int i = 0; i < 10; i++) {
				Vector3f emit = light->sample_Li(sinter, randuv, &wiLight, &pdfLight, &lightSample);
				//	std::cout << emit;
				const Point3f& pLight = lightSample.p;
				const Normal3f& nLight = lightSample.n;
				Ray shadowRay(p, wiLight);
				Vector3f temp = (pLight - p);
				double t = temp.length();
				//判断Shadow Ray是否被场景遮挡
				SurfaceInteraction test;
				if (!scene.intersect(shadowRay, test, 0.000001, t - 0.001)) {
					double det = Clamp(temp.length() * temp.length() * pdfLight, 0.01, 1.0);
					L_dir += emit * sinter.bsdf->f(wo, wiLight) * Dot(wiLight, n) * Dot(-wiLight, nLight) / det;
				}
			}
			L_dir /= 10;

			//采样brdf,间接光照
			double pdf;
			Vector3f f = sinter.bsdf->sample_f(wo, wi, Point2f(GetRand(), GetRand()), &pdf);//sample out direction
			double cosTheta = Clamp(Dot(wi, n), 0.0, 1.0);
			Ray r_out(sinter.p, wi);
			L_ind = f * integrate(scene, r_out, depth + 1) * cosTheta / pdf;

			return L_ind + L_dir;
		}
		return Vector3f(0.0);
	}

	GeometryData SimplifiedRenderer::gBuffer(const Ray& ray, const Scene& scene)const {
		SurfaceInteraction record;
		GeometryData data;

		if (scene.intersect(ray, record, 0.001, std::numeric_limits<double>::max())) {
			const Point3f& p = record.p;
			const Normal3f& n = record.n;
			data.n = record.n;
			data.p = record.p;
			data.hit = true;
		}
		return data;
	}

}