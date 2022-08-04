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
#include<tinyxml/tinyxml2.h>
#include"core/programe.h"
#include"core/camera.h"

using namespace std;
using namespace Raven;

int main(int agrc, char** argv)
{
	//if (agrc == 1) {
	auto start = std::chrono::system_clock::now();
	Raven::Scene box = Raven::Scene::buildCornellBox();
	//Raven::Scene box = Raven::Scene::buildTestScene();
	Raven::Scene sphere = Raven::Scene::buildTestSphere();

	std::shared_ptr<Raven::Film> f = std::make_shared<Film>(400, 400);
	Raven::Transform cameraToWorld = Raven::LookAt(Point3f(278, 273, -800), Point3f(278, 273, 0), Vector3f(0, 1, 0));
	Raven::Transform screenToRaster = Raven::Raster(f->yRes, f->xRes);
	std::shared_ptr<Raven::Camera> cam =
		std::make_shared<Raven::PerspectiveCamera>(cameraToWorld, screenToRaster, 0.0, 865.0, 0.1f, 10.0f, 40.f, f->aspect_ratio);
	std::shared_ptr<Raven::Camera> ocam =
		std::make_shared<Raven::OrthographicCamera>(cameraToWorld, screenToRaster, 0, 10, 278, -278, -278, 278, 2, 100);
	Raven::PathTracingRenderer renderer(cam, f, 50, 100);
	renderer.render(sphere);

	auto stop = std::chrono::system_clock::now();
	std::cout << "Render complete:\n";
	auto hours = std::chrono::duration_cast<std::chrono::hours>(stop - start).count();
	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() - hours * 60;
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() - minutes * 60;

	std::cout << hours << "hours\n";
	std::cout << minutes << "minutes\n";
	std::cout << seconds << "seconds\n";

	return 0;

	//Parser parser;
	//parser.parse("D:/MyWorks/Raven/models/mateball/scene.xml");
	//std::shared_ptr<RavenRenderingPrograme> pipeline = parser.makePrograme();
	//pipeline->render();
	//return 0;
//}
//else {
//	std::cout << "Raven start build scene." << std::endl;
//	std::string path = argv[1];
//	Parser parser;
//	parser.parse(path);
//	std::shared_ptr<RavenRenderingPrograme> pipeline = parser.makePrograme();
//	pipeline->render();
//	return 0;
//
//
//	Transform I = Identity();
//	Transform t = RotateX(45.0);
//
//	Point3f p(1, 0, 0);
//	Point3f pPrime = I(p);
//
//}
}


