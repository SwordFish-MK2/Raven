#ifndef _RAVEN_RENDERER_PATH_TRACING_H_
#define _RAVEN_RENDERER_PATH_TRACING_H_

#include"../core/renderer.h"
#include"../core/base.h"
#include"../core/spectrum.h"
namespace Raven {
	class PathTracingRenderer : public Renderer {
	public:
		PathTracingRenderer(const Camera* c, const Film& f, int spp = 100, int maxDepth = 64, double epslion = 1e-6) :
			Renderer(c, f, spp, epsilon), maxDepth(maxDepth){}
		virtual void render(const Scene& scene);

	private:
		Spectrum integrate(const Scene& scene, const Ray& r_in,int depth=0)const;
	//	GeometryData gBuffer(const Ray& ray, const Scene& scene)const;


		const int maxDepth;
	};
}

#endif