#include <Raven/Raven.h>
#include <Raven/camera/projectiveCamera.h>
#include <Raven/core/camera.h>
#include <Raven/core/film.h>
#include <Raven/core/math.h>
#include <Raven/core/programe.h>
#include <Raven/core/scene.h>
#include <Raven/core/transform.h>
#include <Raven/integrator/path.h>
#include <Raven/sampler/stratified.h>
#include <Raven/shape/mesh.h>
#include <Raven/textute/solidTexture.h>
#include <Raven/utils/factory.h>
#include <tinyxml2.h>

#include <chrono>
#include <memory>

using namespace std;
using namespace Raven;

int main(int agrc, char** argv) {
  ////if (agrc == 1) {
  auto         start = std::chrono::system_clock::now();
  Raven::Scene box   = Raven::Scene::buildCornellBox();

  std::unique_ptr<Raven::Film> f = std::make_unique<Film>(128 * 3, 128 * 3);

  Raven::Transform cameraToWorld = Raven::LookAt(
      Point3f(278, 273, -800), Point3f(278, 273, -799), Vector3f(0, 1, 0));
  Raven::Transform screenToRaster = Raven::Raster(f->yRes, f->xRes);
  Bound2f          viewport{Point2f(-300, -300), Point2f(300, 300)};

  std::unique_ptr<Raven::Camera> cam =
      std::make_unique<Raven::PerspectiveCamera>(cameraToWorld, 0.0, 1e6, 40.f,
                                                 std::move(f), nullptr);

  // std::unique_ptr<Raven::Camera> ocam =
  //     std::make_unique<Raven::OrthographicCamera>(
  //         cameraToWorld, viewport, 0, 10, 0, 1, std::move(f), nullptr);

  std::unique_ptr<Raven::Sampler> sampler =
      std::make_unique<Raven::StratifiedSampler>(5, 5, true, 2);

  Raven::PathTracingIntegrator renderer(std::move(cam), std::move(sampler));
  renderer.render(box);

  auto stop = std::chrono::system_clock::now();
  std::cout << "Render complete:\n";
  auto hours =
      std::chrono::duration_cast<std::chrono::hours>(stop - start).count();
  auto minutes =
      std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() -
      hours * 60;
  auto seconds =
      std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() -
      minutes * 60;

  std::cout << hours << "hours\n";
  std::cout << minutes << "minutes\n";
  std::cout << seconds << "seconds\n";

  return 0;

  // Parser parser;
  // parser.parse("D:/MyWorks/Raven/models/mateball/scene.xml");
  // std::shared_ptr<RavenRenderingPrograme> pipeline = parser.makePrograme();
  // pipeline->render();
  // return 0;
  //}
  // else {
  //	std::cout << "Raven start build scene." << std::endl;
  //	std::string path = argv[1];
  //	Parser parser;
  //	parser.parse(path);
  //	std::shared_ptr<RavenRenderingPrograme> pipeline =
  // parser.makePrograme(); 	pipeline->render(); 	return 0;
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

// int main() {
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
////	Ref<Camera> c =
/// std::dynamic_pointer_cast<Camera>(myfactory.generate("perspective", list));
//
//	//std::cout << decltype(*c);
//
//}