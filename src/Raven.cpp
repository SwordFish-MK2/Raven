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

	Raven::Film f(500, 500, 1.0);
	Raven::Transform cameraToWorld = Raven::Translate(Raven::Vector3f(278, 273, -800));
	Raven::Transform screenToRaster = Raven::Raster(f.height, f.width);
	Raven::Camera* cam = new Raven::PerspectiveCamera(cameraToWorld, screenToRaster, 0.0f, 150.f, 
		10.0f, 1000.0f, 45.0f, f.aspect_ratio);
	//Sampler* sampler = new StratifiedSampler(10, 10, 15, true);
	Raven::SimplifiedRenderer renderer(cam, f,200,64,0.1);
	renderer.render(s);
	s.clear();
	delete cam;
	return 0;
}
