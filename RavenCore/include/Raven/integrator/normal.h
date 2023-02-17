// #ifndef _RAVEN_RENDERER_NORMAL_H_
// #define _RAVEN_RENDERER_NORMAL_H_
//
// #include"../core/base.h"
// #include"../core/renderer.h"
// namespace Raven {
//	class NormalRenderer :public Renderer {
//	public:
//		NormalRenderer(const Camera* c, const Film& f, int spp = 100, int
//maxDepth = 64, double epslion = 1e-6) : 			Renderer(c, f, spp, epsilon) {}
//
//		virtual void render(const Scene& scene);
//
//	private:
//		Vector3f integrate(const Scene& scene, const Ray& r_in, int depth
//= 0)const;
//	};
// }
// #endif
