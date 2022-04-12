// Raven.cpp: 定义应用程序的入口点。
//

#include "Raven.h"
#include"core/math.h"
#include<Eigen/Eigen>
#include"core/transform.h"
#include"camera/projectiveCamera.h"
#include"core/scene.h"
#include"core/film.h"
#include"camera/projectiveCamera.h"
#include"renderer/path.h"
#include<chrono>
#include"renderer/normal.h"
#include"texture/solidTexture.h"
using namespace std;

int main()
{
	auto start = std::chrono::system_clock::now();
	//Raven::Scene box = Raven::Scene::buildCornellBox();
	//Raven::Scene box = Raven::Scene::buildTestScene();
	Raven::Scene sphere = Raven::Scene::buildTestSphere();


	Raven::Film f(500, 500);
	Raven::Transform cameraToWorld = Raven::Translate(Raven::Vector3f(278, 273, -800));
	Raven::Transform screenToRaster = Raven::Raster(f.height, f.width);
	Raven::Camera* cam = new Raven::PerspectiveCamera(cameraToWorld, screenToRaster, 0.0f, 150.f,
		10.0f, 1000.0f, 40.f, f.aspect_ratio);
	//Sampler* sampler = new StratifiedSampler(10, 10, 15, true);
	Raven::PathTracingRenderer renderer(cam, f, 20, 10, 0.1);
	//Raven::NormalRenderer nRenderer(cam, f, 10, 1, 0.00001);
	renderer.render(sphere);
	//nRenderer.render(sphere);
	delete cam;

	auto stop = std::chrono::system_clock::now();
	std::cout << "Render complete:\n";
	auto hours = std::chrono::duration_cast<std::chrono::hours>(stop - start).count();
	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() - hours * 60;
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() - minutes * 60;

	std::cout << hours << "hours\n";
	std::cout << minutes << "minutes\n";
	std::cout << seconds << "seconds\n";
	return 0;

}
