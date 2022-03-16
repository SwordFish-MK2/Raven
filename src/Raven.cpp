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
#include"renderer/simplified.h"
using namespace std;

int main()
{
	Raven::Scene s;
	s.init();

	Raven::Film f(400, 225, 16.0f / 9.0f);
	Raven::Transform cameraToWorld = Raven::Transform::Identity();
	Raven::Transform screenToRaster = Raven::Transform::Raster(f.height, f.width);
	Raven::Camera* cam = new Raven::PerspectiveCamera(cameraToWorld, screenToRaster, 0.0f, 150.f, 
		100.0f, 500.0f, 90.0f, f.aspect_ratio);
	//Sampler* sampler = new StratifiedSampler(10, 10, 15, true);
	Raven::SimplifiedRenderer renderer(cam, f,200,50,0.1);
	renderer.render(s);
	s.clear();
	delete cam;
	return 0;
}
