#ifndef _RAVEN_RENDERER_TEST_H_
#define _RAVEN_RENDERER_TEST_H_

#include"../core/base.h"
#include"../core/renderer.h"
#include"../core/material.h"
#include"../core/spectrum.h"

namespace Raven {
	class SimplifiedRenderer :public Renderer {
	public:
		SimplifiedRenderer(Camera* c,const Film& f, int spp, int maxDepth)
			:Renderer(c, f, spp), maxDepth(maxDepth) {}
		//generate ray and sample the scene, output final image
		void render(const Scene& scene);
	private:
		//perform rendering equation by path tracing
		//Spectrum integrate(const Ray& r_in, const Scene& scene, int depth = 0)const;
		Spectrum integrate(const Ray& r_in, const Scene& scene, int depth = 0)const;
		const double epsilon = 0.1;
		const int maxDepth;
	};
}

#endif