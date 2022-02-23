#ifndef _RAVEN_CORE_RENDERER_H_
#define _RAVEN_CORE_RENDERER_H_

#include"base.h"
#include"camera.h"
#include"film.h"
#include"scene.h"

namespace Raven {
	class Renderer {
	public:
		Renderer(const Camera* c, const Film& f,int spp=100) :camera(c), film(f),spp(spp) {}
		virtual void render(const Scene& scene) = 0;
	protected:
		const Camera* camera;
		Film film;
		const int spp;
	};
}

#endif