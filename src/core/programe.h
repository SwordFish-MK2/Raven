#ifndef _RAVEN_CORE_RENDERING_PROGRAME_H_
#define _RAVEN_CORE_RENDERING_PROGRAME_H_

#include"camera.h"
#include"renderer.h"
#include"scene.h"
#include<chrono>
#include<map>
namespace Raven {
	class RavenRenderingPrograme {
	public:
		RavenRenderingPrograme(
			const std::shared_ptr<Renderer>& r,
			const std::shared_ptr<Scene>& s,
			const std::vector<std::shared_ptr<Transform>>& t) :
			renderer(r), scene(s),t(t) {}
	private:
		std::shared_ptr<Renderer> renderer;
		std::shared_ptr<Scene> scene;
		std::vector<std::shared_ptr<Transform>> t;
	public:
		void render();
	};

	inline void RavenRenderingPrograme::render() {
		std::cout << "Rendering begin.\n";
		auto start = std::chrono::system_clock::now();
		renderer->render(*scene);
		auto stop = std::chrono::system_clock::now();
		std::cout << "Rendering complete:\n";
		auto hours = std::chrono::duration_cast<std::chrono::hours>(stop - start).count();
		auto minutes = std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() - hours * 60;
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() - minutes * 60;

		std::cout << hours << "hours\n";
		std::cout << minutes << "minutes\n";
		std::cout << seconds << "seconds\n";
	}
}

#endif