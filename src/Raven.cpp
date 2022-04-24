// Raven.cpp: 定义应用程序的入口点。
//

#include "Raven.h"
#include"core/math.h"
#include"core/transform.h"
#include"camera/projectiveCamera.h"
#include"core/scene.h"
#include"core/film.h"
#include"camera/projectiveCamera.h"
#include"renderer/path.h"
#include<chrono>
#include"renderer/normal.h"
#include"texture/solidTexture.h"
#include"shape/mesh.h"
#include"core/math.h"

using namespace std;
using namespace Raven;

int main()
{
	auto start = std::chrono::system_clock::now();
	//Raven::Scene box = Raven::Scene::buildCornellBox();	
	Raven::Scene box = Raven::Scene::buildTestScene();
	//Raven::Scene sphere = Raven::Scene::buildTestSphere();

	Raven::Film f(500, 500);
	Raven::Transform cameraToWorld = Raven::LookAt(Point3f(278, 273, -800), Point3f(278, 273, 0), Vector3f(0, 1, 0));
	Raven::Transform screenToRaster = Raven::Raster(f.height, f.width);
	Raven::Camera* cam = new Raven::PerspectiveCamera(cameraToWorld, screenToRaster, 0.0, 865.0,
		0.1f, 10.0f, 40.f, f.aspect_ratio);
	Raven::Camera* ocam = new Raven::OrthographicCamera(cameraToWorld, screenToRaster, 0, 10, 278, -278, -278, 278, 2, 100);
	Raven::PathTracingRenderer renderer(cam, f, 40, 10);
	renderer.render(box);
	delete cam;
	delete ocam;

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

