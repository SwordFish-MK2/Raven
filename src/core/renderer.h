#ifndef _RAVEN_CORE_RENDERER_H_
#define _RAVEN_CORE_RENDERER_H_

#include"base.h"
#include"camera.h"
#include"film.h"
#include"scene.h"
#include"interaction.h"
#include"spectrum.h"

namespace Raven {
	class Renderer {
	public:
		Renderer(const std::shared_ptr<Camera>& c,
			const std::shared_ptr<Film>& f,
			int spp = 100,
			double epsilon = 0.0001) :camera(c), film(f), spp(spp), epsilon(epsilon) {}
		virtual void render(const Scene& scene) = 0;
	protected:
		const double epsilon;
		std::shared_ptr<Camera> camera;
		std::shared_ptr<Film> film;
		const int spp;
	};
	//����MIS weight
	inline double PowerHeuristic(int fNum, double fPdf, int gNum, double gPdf) {
		double f = fNum * fPdf;
		double g = gNum * gPdf;
		return f * f / (f * f + g * g);
	}

	Spectrum EvaluateLight(const SurfaceInteraction& record, const Scene& scene, const Light& light);

	Spectrum SampleOneLight(const SurfaceInteraction& record, const Scene& scene, int nSample);

	Spectrum SampleAllLights(const SurfaceInteraction& record, const Scene& scene);
}

#endif