#include<Raven/Raven.h>
#include<Raven/core/math.h>
#include<Raven/core/transform.h>
#include<Raven/camera/projectiveCamera.h>
#include<Raven/core/scene.h>
#include"Raven/core/film.h"
#include"Raven/camera/projectiveCamera.h"
#include"Raven/integrator/path.h"
#include<chrono>
#include<Raven/textute/solidTexture.h>
#include<Raven/shape/mesh.h>
#include<Raven/core/math.h>
#include<tinyxml2.h>
#include<Raven/core/programe.h>
#include<Raven/core/camera.h>
#include<Raven/utils/factory.h>

using namespace std;
using namespace Raven;

int main(int agrc, char** argv)
{
	////if (agrc == 1) {
	//auto start = std::chrono::system_clock::now();
	//Raven::Scene box = Raven::Scene::buildCornellBox();
	////Raven::Scene box = Raven::Scene::buildTestScene();
	//Raven::Scene sphere = Raven::Scene::buildTestSphere();

	//std::shared_ptr<Raven::Film> f = std::make_shared<Film>(128 * 3, 128 * 3);
	////Eigen::Matrix4f cw;
	////cw << 0. - 721367, -0.373123, -0.583445, -0,
	////	-0, 0.842456, -0.538765, -0,
	////	-0.692553, -0.388647, -0.60772, -0,
	////	0.0258668, -0.29189, 5.43024, 1;
	////auto cwt = cw.transpose();
	////Raven::Transform cameraWorld(cwt);
	////Raven::Transform cameraToWorld = Raven::LookAt(Point3f(3, 3, 3), Point3f(0, 0.25, 0), Vector3f(0, 1, 0));

	//Raven::Transform cameraToWorld = Raven::LookAt(Point3f(278, 273, -800), Point3f(278, 273, -799), Vector3f(0, 1, 0));
	//Raven::Transform screenToRaster = Raven::Raster(f->yRes, f->xRes);
	//Vector2f viewPort{ 128 * 3,128 * 3 };
	//std::shared_ptr<Raven::Camera> cam =
	//	std::make_shared<Raven::PerspectiveCamera>(cameraToWorld, viewPort, 0.0, 1e6,40.f, f, nullptr);

	//std::shared_ptr<Raven::Camera> ocam =
	//	std::make_shared<Raven::OrthographicCamera>(cameraToWorld, viewPort, 0, 10, 0, 1, f, nullptr);
	//Raven::PathTracingIntegrator renderer(10, 2);
	//renderer.render(box, cam, f);

	//auto stop = std::chrono::system_clock::now();
	//std::cout << "Render complete:\n";
	//auto hours = std::chrono::duration_cast<std::chrono::hours>(stop - start).count();
	//auto minutes = std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() - hours * 60;
	//auto seconds = std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() - minutes * 60;

	//std::cout << hours << "hours\n";
	//std::cout << minutes << "minutes\n";
	//std::cout << seconds << "seconds\n";

	//return 0;

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

	Mat4f m;
	m << 1, 0, 1, 0,
		0, 1, 0, 1,
		1, 0, 0, 1,
		0, 0, 1, 0;
	std::cout << m;
}

//int main() {
//	PropertyList list;
//	//list.setInteger("width", 1080);
//	//list.setInteger("height", 720);
//
//	list.setInteger("spp", 100);
//	list.setInteger("maxDepth", 20);
//	list.setFloat("epsilon", 1e-6);
//	Factory& myfactory = Factory::getInctance();
//
//	myfactory.regClass("film", Film::construct);
//
//	myfactory.regClass("perspective", PerspectiveCamera::construct);
//
//	//Ref<RavenObject> obj = myfactory.generate("film", list);
//	Ref<Transform> ctw = std::make_shared<Transform>(Identity());
//	Ref<Transform> str = ctw;
//
//	//Ref<Film> f = std::dynamic_pointer_cast<Film>(obj);
//
//
//	list.setObjectRef("transform", ctw);
//	list.setObjectRef("transform", str);
//
//
//
//
////	Ref<Camera> c = std::dynamic_pointer_cast<Camera>(myfactory.generate("perspective", list));
//
//	//std::cout << decltype(*c);
//
//}