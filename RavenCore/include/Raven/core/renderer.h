#ifndef _RAVEN_CORE_RENDERER_H_
#define _RAVEN_CORE_RENDERER_H_

#include<Raven/core/base.h>
#include<Raven/core/camera.h>
#include<Raven/core/film.h>
#include<Raven/core/scene.h>
#include<Raven/core/interaction.h>
#include<Raven/core/spectrum.h>
#include<Raven/core/bsdf.h>
#include<Raven/core/object.h>

namespace Raven {
	class Renderer:public RavenObject {
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
	//º∆À„MIS weight
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