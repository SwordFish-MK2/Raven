#include"scene.h"
#include"../material/matte.h"
#include"../shape/sphere.h"
#include"../shape/mesh.h"
#include"../light/areaLight.h"
#include"../utils/loader.h"
#include"../texture/solidTexture.h"
#include"../material/plastic.h"
#include"../core/Spectrum.h"
#include"../texture/mapping.h"
#include"../accelerate/bvh.h"
#include"../texture/imageTexture.h"
#include"../material/mirror.h"
#include"../material/glass.h"
#include"mipmap.h"
#include<string>

namespace Raven {

	Scene::Scene(
		const std::vector<std::shared_ptr<Transform>>& trans,
		const std::vector<std::shared_ptr<Light>>& lights,
		const std::vector<std::shared_ptr<TriangleMesh>>& meshes,
		const std::vector<std::shared_ptr<Primitive>>& prims,
		AccelType type) :
		meshes(meshes), lights(lights), transforms(trans) {

		switch (type) {
		case AccelType::List:
			objs = std::make_shared<PrimitiveList>(prims);
			break;
		case AccelType::KdTree:
			objs = std::make_shared<KdTreeAccel>(prims, 10, 80, 1, 0.5, 5);
			break;
		case AccelType::BVH:
			objs = std::make_shared<BVHAccel>(prims, 1);
			break;
		}

	}

	Scene::Scene(const Scene& s) :transforms(s.transforms),
		lights(s.lights), meshes(s.meshes), objs(s.objs) {}

	//在场景中添加网格
	void Scene::addMesh(const std::shared_ptr<TriangleMesh>& mesh_ptr) {
		meshes.push_back(mesh_ptr);
	}

	//在场景中添加矩阵
	void Scene::addTransform(const std::shared_ptr<Transform>& trans_ptr) {
		transforms.push_back(trans_ptr);
	}

	//在场景中添加光源
	void Scene::addLight(const std::shared_ptr<Light>& light_ptr) {
		lights.push_back(light_ptr);
	}

	Scene Scene::buildCornellBox() {
		std::vector<std::shared_ptr<TriangleMesh>> meshes;
		std::vector<std::shared_ptr<Transform>> usedTransform;
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<Primitive>> prim_ptrs;

		//transform
		std::shared_ptr<Transform> identity = std::make_shared<Transform>(Identity());
		usedTransform.push_back(identity);

		Loader loader;

		//right wall
		{
			std::optional<TriangleInfo> rightInfo =
				loader.load("D:/MyWorks/Raven/models/cornellbox/", "right.obj");
			std::shared_ptr<TriangleMesh> rightMesh =
				TriangleMesh::build(identity.get(), identity.get(), *rightInfo);
			meshes.push_back(rightMesh);
			const auto& triangles = rightMesh->triangles;

			//material of right wall
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.14f, 0.45f, 0.091f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		}

		//left wall
		{
			//load mesh
			std::optional<TriangleInfo> leftInfo =
				loader.load("D:/MyWorks/Raven/models/cornellbox/", "left.obj");
			std::shared_ptr<TriangleMesh> leftMesh =
				TriangleMesh::build(identity.get(), identity.get(), *leftInfo);
			meshes.push_back(leftMesh);
			const auto& triangles = leftMesh->triangles;
			//material of left wall
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.63f, 0.065f, 0.05f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		}

		//floor
		{
			//load mesh
			std::optional<TriangleInfo> floorInfo =
				loader.load("D:/MyWorks/Raven/models/cornellbox/", "floorOriginal.obj");
			std::shared_ptr<TriangleMesh> floorMesh =
				TriangleMesh::build(identity.get(), identity.get(), *floorInfo);
			meshes.push_back(floorMesh);
			const auto& triangles = floorMesh->triangles;

			//material of floor
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.725f, 0.71f, 0.68f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		}

		//tall box
		{
			//load mesh
			std::optional<TriangleInfo> tBoxInfo =
				loader.load("D:/MyWorks/Raven/models/cornellbox/", "tallbox.obj");
			std::shared_ptr<TriangleMesh> tBoxMesh =
				TriangleMesh::build(identity.get(), identity.get(), *tBoxInfo);
			meshes.push_back(tBoxMesh);
			const auto& triangles = tBoxMesh->triangles;

			//material of short box
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.725f, 0.71f, 0.68f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		}

		//short box
		{
			//load mesh
			std::optional<TriangleInfo> sBoxInfo =
				loader.load("D:/MyWorks/Raven/models/cornellbox/", "shortbox.obj");
			std::shared_ptr<TriangleMesh> sBoxMesh =
				TriangleMesh::build(identity.get(), identity.get(), *sBoxInfo);
			meshes.push_back(sBoxMesh);
			const auto& triangles = sBoxMesh->triangles;
			//material of short box
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.725f, 0.71f, 0.68f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));

		}


		//Light
		{
			//load light mesh
			std::optional<TriangleInfo> lightInfo =
				loader.load("D:/MyWorks/Raven/models/cornellbox/", "light.obj");
			std::shared_ptr<TriangleMesh> lightMesh =
				TriangleMesh::build(identity.get(), identity.get(), *lightInfo);
			meshes.push_back(lightMesh);
			const auto& triangles = lightMesh->triangles;

			//light material
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.65f));

			//light emittion
			Spectrum lightEmit = RGBSpectrum::fromRGB(8.0 * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f)
				+ 15.6 * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4 *
				Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f));

			//generate light primitive
			for (size_t i = 0; i < triangles.size(); i++) {
				auto l = std::make_shared<DiffuseAreaLight>(identity.get(), identity.get(), 5, triangles[i].get(), lightEmit);
				auto p = std::make_shared<Primitive>(triangles[i], mate, l);
				lights.push_back(l);//adding light ptr to scene instance
				prim_ptrs.push_back(p);//adding pritive 
			}
		}
		return Scene(usedTransform, lights, meshes, prim_ptrs, AccelType::BVH);

	}

	//Scene Scene::buildTestScene() {
	//	std::vector<std::shared_ptr<TriangleMesh>> meshes;
	//	std::vector<std::shared_ptr<Transform>> usedTransform;
	//	std::vector<std::shared_ptr<Light>> lights;
	//	std::vector<std::shared_ptr<Primitive>> prim_ptrs;

	//	std::shared_ptr<Transform> identity = std::make_shared<Transform>(Identity());
	//	std::shared_ptr<Transform> sphereWorld = std::make_shared<Transform>(Translate(Vector3f(200, 100, 190)));
	//	std::shared_ptr<Transform> invSphereWorld = std::make_shared<Transform>(sphereWorld->inverse());
	//	Transform bunnyScale = Scale(Vector3f(2000, 2000, 2000));
	//	std::shared_ptr<Transform> bunnyWorld = std::make_shared<Transform>((*sphereWorld) * bunnyScale);
	//	//std::cout << bunnyWorld->getMatrix();
	//	std::shared_ptr<Transform> invBunnyWorld = std::make_shared<Transform>(bunnyWorld->inverse());

	//	usedTransform.push_back(identity);
	//	usedTransform.push_back(sphereWorld);
	//	usedTransform.push_back(invSphereWorld);
	//	usedTransform.push_back(bunnyWorld);
	//	usedTransform.push_back(invBunnyWorld);

	//	std::shared_ptr<Shape> sphere = Sphere::build(sphereWorld.get(), invSphereWorld.get(), 90);

	//	Loader loader;
	//	std::string path("D:/MyWorks/Raven/models/cornellbox/");
	//	std::optional<TriangleInfo> leftInfo =
	//		loader.load("D:/MyWorks/Raven/models/cornellbox/", "left.obj");
	//	std::optional<TriangleInfo> rightInfo =
	//		loader.load("D:/MyWorks/Raven/models/cornellbox/", "right.obj");
	//	std::optional<TriangleInfo> floorInfo =
	//		loader.load("D:/MyWorks/Raven/models/cornellbox/", "floor.obj");
	//	std::optional<TriangleInfo> sBoxInfo =
	//		loader.load("D:/MyWorks/Raven/models/cornellbox/", "shortbox.obj");
	//	std::optional<TriangleInfo> tBoxInfo =
	//		loader.load("D:/MyWorks/Raven/models/cornellbox/", "tallbox.obj");
	//	std::optional<TriangleInfo> lightInfo =
	//		loader.load("D:/MyWorks/Raven/models/cornellbox/", "light.obj");
	//	std::optional<TriangleInfo> backInfo =
	//		loader.load(path, "back.obj");
	//	std::optional<TriangleInfo> topInfo =
	//		loader.load(path, "top.obj");
	//	std::optional<TriangleInfo> bunnyInfo =
	//		loader.load("D:/MyWorks/Raven/models/bunny/", "bunny.obj");

	//	std::shared_ptr<TriangleMesh> bunnyMesh =
	//		TriangleMesh::build(bunnyWorld.get(), invBunnyWorld.get(), *bunnyInfo);

	//	std::shared_ptr<TriangleMesh> leftMesh =
	//		TriangleMesh::build(identity.get(), identity.get(), *leftInfo);

	//	std::shared_ptr<TriangleMesh> rightMesh =
	//		TriangleMesh::build(identity.get(), identity.get(), *rightInfo);

	//	std::shared_ptr<TriangleMesh> floorMesh =
	//		TriangleMesh::build(identity.get(), identity.get(), *floorInfo);

	//	std::shared_ptr<TriangleMesh> sBoxMesh =
	//		TriangleMesh::build(identity.get(), identity.get(), *sBoxInfo);

	//	std::shared_ptr<TriangleMesh> tBoxMesh =
	//		TriangleMesh::build(identity.get(), identity.get(), *tBoxInfo);

	//	std::shared_ptr<TriangleMesh> lightMesh =
	//		TriangleMesh::build(identity.get(), identity.get(), *lightInfo);

	//	std::shared_ptr<TriangleMesh> backMesh =
	//		TriangleMesh::build(identity.get(), identity.get(), *backInfo);

	//	std::shared_ptr<TriangleMesh> topMesh =
	//		TriangleMesh::build(identity.get(), identity.get(), *topInfo);

	//	meshes.push_back(leftMesh);
	//	meshes.push_back(rightMesh);
	//	meshes.push_back(floorMesh);
	//	meshes.push_back(sBoxMesh);
	//	meshes.push_back(tBoxMesh);
	//	meshes.push_back(lightMesh);
	//	meshes.push_back(backMesh);
	//	meshes.push_back(topMesh);
	//	meshes.push_back(bunnyMesh);

	//	//Texture
	//	std::shared_ptr<TextureMapping2D> mapping = UVMapping2D::build();
	//	std::shared_ptr<Texture<Spectrum>> whiteTexture = ConstTexture<Spectrum>::build(RGBSpectrum::fromRGB(0.80, 0.80, 0.80));
	//	std::shared_ptr<Texture<Spectrum>> mirrorR = ConstTexture<Spectrum>::build(RGBSpectrum::fromRGB(0.85, 0.85, 0.85));
	//	std::shared_ptr<Texture<Spectrum>> greenTexture = ConstTexture<Spectrum>::build(RGBSpectrum::fromRGB(0.843137, 0.843137, 0.091f));
	//	std::shared_ptr<Texture<Spectrum>> blackTexture = ConstTexture<Spectrum>::build(RGBSpectrum::fromRGB(0.01, 0.01, 0.01));
	//	std::shared_ptr<Texture<Spectrum>> cheTex = CheckeredTexture<Spectrum>::build(whiteTexture, blackTexture, mapping);
	//	std::shared_ptr<Texture<double>> sigma = ConstTexture<double>::build(0.0);

	//	std::shared_ptr<Texture<Spectrum>> kdTex = ConstTexture<Spectrum>::build(RGBSpectrum::fromRGB(0.725, 0.71f, 0.68f));
	//	std::shared_ptr<Texture<Spectrum>> ksTex = ConstTexture<Spectrum>::build(RGBSpectrum::fromRGB(0.8f, 0.8f, 0.8f));
	//	std::shared_ptr<Texture<double>> roughTex = ConstTexture<double>::build(0.05);

	//	//Material
	//	std::shared_ptr<MatteMaterial> mate1 = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.1, 0.97, 0.4));
	//	std::shared_ptr<MatteMaterial> mate2 = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.5, 0.5, 0.5));
	//	std::shared_ptr<MatteMaterial> redLam = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.63f, 0.065f, 0.05f));
	//	std::shared_ptr<MatteMaterial> greenLam = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.14f, 0.45f, 0.091f));
	//	std::shared_ptr<MatteMaterial> whiteLam = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.725f, 0.71f, 0.68f));
	//	std::shared_ptr<MatteMaterial> lightLam = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.65f));
	//	std::shared_ptr<MatteMaterial> checkered = MatteMaterial::build(sigma, cheTex);

	//	std::shared_ptr<Material> mirror = Mirror::build(mirrorR);
	//	std::shared_ptr<Material> glass = Glass::build(whiteTexture, whiteTexture, sigma, sigma, nullptr, 1.5);
	//	std::shared_ptr<Plastic> plastic = Plastic::build(kdTex, ksTex, roughTex);

	//	//Light
	//	std::vector<std::shared_ptr<Triangle>> sTri = lightMesh->getTriangles();
	//	Spectrum lightEmit = RGBSpectrum::fromRGB(8.0 * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f)
	//		+ 15.6 * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4 *
	//		Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f));
	//	std::shared_ptr<DiffuseAreaLight> aLight1 =
	//		std::make_shared<DiffuseAreaLight>(identity.get(), identity.get(), 5, sTri[0].get(), lightEmit);
	//	std::shared_ptr<DiffuseAreaLight> aLight2 =
	//		std::make_shared<DiffuseAreaLight>(identity.get(), identity.get(), 5, sTri[1].get(), lightEmit);
	//	std::shared_ptr<Primitive> lightp1 = std::make_shared<Primitive>(sTri[0], lightLam, aLight1);
	//	std::shared_ptr<Primitive> lightp2 = std::make_shared<Primitive>(sTri[1], lightLam, aLight2);
	//	lights.push_back(aLight1);
	//	lights.push_back(aLight2);

	//	//Primitives
	//	std::shared_ptr<Primitive> spherePrim = Primitive::build(sphere, whiteLam, nullptr);
	//	std::vector<std::shared_ptr<Primitive>> leftPrim = leftMesh->generatePrimitive(redLam);
	//	std::vector<std::shared_ptr<Primitive>> rightPrim = rightMesh->generatePrimitive(greenLam);
	//	std::vector<std::shared_ptr<Primitive>> floorPrim = floorMesh->generatePrimitive(whiteLam);
	//	std::vector<std::shared_ptr<Primitive>> sBoxPrim = sBoxMesh->generatePrimitive(whiteLam);
	//	std::vector<std::shared_ptr<Primitive>> tBoxPrim = tBoxMesh->generatePrimitive(whiteLam);
	//	std::vector<std::shared_ptr<Primitive>> backPrim = backMesh->generatePrimitive(whiteLam);
	//	std::vector<std::shared_ptr<Primitive>> topPrim = topMesh->generatePrimitive(whiteLam);
	//	std::vector<std::shared_ptr<Primitive>> bunnyPrim = bunnyMesh->generatePrimitive(whiteLam);


	//	//tBoxPrim[8]->setMaterial(mirror);
	//	//tBoxPrim[9]->setMaterial(mirror);

	//	prim_ptrs.push_back(lightp1);
	//	prim_ptrs.push_back(lightp2);
	//	//prim_ptrs.push_back(spherePrim);

	//	prim_ptrs.insert(prim_ptrs.end(), leftPrim.begin(), leftPrim.end());
	//	prim_ptrs.insert(prim_ptrs.end(), bunnyPrim.begin(), bunnyPrim.end());//兔子模型
	//	prim_ptrs.insert(prim_ptrs.end(), rightPrim.begin(), rightPrim.end());
	//	prim_ptrs.insert(prim_ptrs.end(), floorPrim.begin(), floorPrim.end());
	//	prim_ptrs.insert(prim_ptrs.end(), backPrim.begin(), backPrim.end());
	//	prim_ptrs.insert(prim_ptrs.end(), topPrim.begin(), topPrim.end());
	//	//prim_ptrs.insert(prim_ptrs.end(), sBoxPrim.begin(), sBoxPrim.end());
	//	//prim_ptrs.insert(prim_ptrs.end(), tBoxPrim.begin(), tBoxPrim.end());

	//	return Scene(usedTransform, lights, meshes, prim_ptrs, AccelType::BVH);

	//}

	Scene Scene::buildTestSphere() {
		std::vector<std::shared_ptr<TriangleMesh>> meshes;
		std::vector<std::shared_ptr<Transform>> usedTransform;
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<Primitive>> prim_ptrs;

		std::shared_ptr<Transform> identity = std::make_shared<Transform>(Identity());
		usedTransform.push_back(identity);

		//sphere
		{
			std::shared_ptr<Transform> sphereWorld = std::make_shared<Transform>(Translate(Vector3f(278, 200, 400)));
			std::shared_ptr<Transform> invSW = std::make_shared<Transform>(sphereWorld->inverse());
			usedTransform.push_back(sphereWorld);
			usedTransform.push_back(invSW);

			const auto shape = Sphere::build(sphereWorld.get(),invSW.get(),190);
			auto mapping = std::make_shared<UVMapping2D>();
			std::shared_ptr<ImageTexture<RGBSpectrum,Spectrum>> kd = std::make_shared<ImageTexture<RGBSpectrum,Spectrum>>
				("C:/Users/HJW/Desktop/earthmap.jpg",mapping, true, ImageWrap::ImClamp, true);
			std::shared_ptr<Texture<double>> sigma = ConstTexture<double>::build(0.0);
			//const auto material = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.5));
			const auto material = std::make_shared<MatteMaterial>(sigma, kd);
			prim_ptrs.push_back(std::make_shared<Primitive>(shape, material));
		}

		//floor
		{
			Point3f p0(-10000.0, -2.0, 10000.0);
			Point3f p1(10000.0, -2.0, 10000.0);
			Point3f p2(10000.0, -2.0, -10000.0);
			Point3f p3(-10000.0, -2.0, -10000.0);

			std::shared_ptr<TriangleMesh> mesh = CreatePlane(identity.get(), identity.get(),
				p0, p1, p2, p3, Normal3f(0.0, 1.0, 0.0));
			meshes.push_back(mesh);
			const auto shapes = mesh->triangles;
			const auto material = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.5));

			for (size_t i = 0; i < shapes.size(); i++) {
				auto p = std::make_shared<Primitive>(shapes[i], material);
				prim_ptrs.push_back(p);
			}
		}

		//light
		{
			Point3f pl0(443.0, 848.7, 127.0);
			Point3f pl1(443.0, 848.7, 232.0);
			Point3f pl2(313.0, 848.7, 232.0);
			Point3f pl3(313.0, 848.7, 127.0);

			std::shared_ptr<TriangleMesh> lightMesh = CreatePlane(identity.get(), identity.get(),
				pl0, pl1, pl2, pl3, Normal3f(0.0, -1.0, 0.0));
			meshes.push_back(lightMesh);
			auto shapes = lightMesh->triangles;
			const Spectrum lightEmit = RGBSpectrum::fromRGB(47.0, 47.0, 47.0);
			const auto material = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.5));

			for (size_t i = 0; i < shapes.size(); i++) {
				auto l = std::make_shared<DiffuseAreaLight>(identity.get(), identity.get(), 5, shapes[i].get(), lightEmit);
				auto p = std::make_shared<Primitive>(shapes[i], material, l);
				lights.push_back(l);
				prim_ptrs.push_back(p);
			}
		}

		return Scene(usedTransform, lights, meshes, prim_ptrs, AccelType::BVH);
	}


}