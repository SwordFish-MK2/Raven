﻿#include<Raven/Raven.h>
#include<Raven/core/math.h>
#include<Raven/core/transform.h>
#include<Raven/camera/projectiveCamera.h>
#include<Raven/core/scene.h>
#include"Raven/core/film.h"
#include"Raven/camera/projectiveCamera.h"
#include"Raven/renderer/path.h"
#include<chrono>
#include<Raven/renderer/normal.h>
#include<Raven/textute/solidTexture.h>
#include<Raven/shape/mesh.h>
#include<Raven/core/math.h>
#include<tinyxml2.h>
#include<Raven/core/programe.h>
#include<Raven/core/camera.h>

using namespace std;
using namespace Raven;

int main(int agrc, char** argv)
{
	//if (agrc == 1) {
	auto start = std::chrono::system_clock::now();
	Raven::Scene box = Raven::Scene::buildCornellBox();
	//Raven::Scene box = Raven::Scene::buildTestScene();
	Raven::Scene sphere = Raven::Scene::buildTestSphere();

	std::shared_ptr<Raven::Film> f = std::make_shared<Film>(128*5, 72*5);
	Eigen::Matrix4f cw;
	cw << 0.-721367, -0.373123, -0.583445, -0,
		-0, 0.842456, -0.538765, -0,
		-0.692553, -0.388647, -0.60772, -0,
		0.0258668, -0.29189, 5.43024, 1;
	auto cwt = cw.transpose();
	Raven::Transform cameraWorld(cwt);
	Raven::Transform cameraToWorld = Raven::LookAt(Point3f(3, 3, 3), Point3f(0, 0.25, 0), Vector3f(0, 1, 0));
	Raven::Transform screenToRaster = Raven::Raster(f->yRes, f->xRes);
	std::shared_ptr<Raven::Camera> cam =
		std::make_shared<Raven::PerspectiveCamera>(cameraToWorld, screenToRaster, 0.0, 1e6, 1e-2, 1000, 40.f, 1);
	std::shared_ptr<Raven::Camera> ocam =
		std::make_shared<Raven::OrthographicCamera>(cameraToWorld, screenToRaster, 0, 10, 278, -278, -278, 278, 2, 100);
	Raven::PathTracingRenderer renderer(cam, f, 100, 2);
	renderer.render(box);

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