#ifndef _RAVEN_CORE_RENDERER_H_
#define _RAVEN_CORE_RENDERER_H_

#include"base.h"
#include"camera.h"
#include"film.h"
#include"scene.h"
#include"interaction.h"
namespace Raven {
	class Renderer {
	public:
		Renderer(const Camera* c, const Film& f,int spp=100,double epsilon=0.0001) :camera(c), film(f),spp(spp),epsilon(epsilon) {}
		virtual void render(const Scene& scene) = 0;
	protected:
		const double epsilon;
		const Camera* camera;
		Film film;
		const int spp;
	};
	//º∆À„MIS weight
	inline double PowerHeuristic(int fNum, double fPdf, int gNum, double gPdf) {
		double f = fNum * fPdf;
		double g = gNum * gPdf;
		return f * f / (f * f + g * g);
	}

	Vector3f EvaluateLight(const SurfaceInteraction& record, const Scene& scene, const Light& light);

	Vector3f SampleOneLight(const SurfaceInteraction& record, const Scene& scene, int nSample);

	Vector3f SampleAllLights(const SurfaceInteraction& record, const Scene& scene);
}

#endif