#include<Raven/core/scene.h>
#include<Raven/material/glass.h>
#include<Raven/material/matte.h>
#include<Raven/material/mirror.h>
#include<Raven/material/plastic.h>
#include<Raven/shape/sphere.h>
#include<Raven/shape/mesh.h>
#include<Raven/shading/lambertain.h>
#include<Raven/utils/loader.h>
#include<Raven/textute/solidTexture.h>
#include<Raven/core/spectrum.h>
#include<Raven/textute/mapping.h>
#include<Raven/accelerate/bvh.h>
#include<Raven/textute/imageTexture.h>
#include<Raven/light/infiniteAreaLight.h>
#include<Raven/core/mipmap.h>
#include<Raven/light/areaLight.h>
#include<string>

namespace Raven {

	Scene::Scene(
		const std::vector<std::shared_ptr<Transform>>& trans,
		const std::vector<std::shared_ptr<Light>>& lights,
		const std::vector<std::shared_ptr<TriangleMesh>>& meshes,
		const std::vector<std::shared_ptr<Primitive>>& prims,
		const std::vector<std::shared_ptr<InfiniteAreaLight>>& areal,
		AccelType type) :
		meshes(meshes), lights(lights), transforms(trans), infinitAreaLights(areal) {

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
		std::vector<std::shared_ptr<InfiniteAreaLight>> areal;
		//transform
		std::shared_ptr<Transform> identity = std::make_shared<Transform>(Identity());
		usedTransform.push_back(identity);

		Loader loader;

		//right wall
		{
			std::optional<TriangleInfo> info =
				loader.load("/models/cornellbox/right.obj");
			std::shared_ptr<TriangleMesh> mesh =
				std::make_shared<TriangleMesh>(identity, identity,
					info->numbers, info->vertices, info->indices,
					info->normals, info->tangants, info->uvs);
			meshes.push_back(mesh);
			const auto& triangles = mesh->triangles;

			//material of right wall
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.14f, 0.45f, 0.091f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		}

		//left wall
		{
			//load mesh
			//std::optional<TriangleInfo> info =
			//	loader.load(PATH + "/models/cornellbox/", "cbox_redwall.obj");
			std::optional<TriangleInfo> info =
				loader.load("/models/cornellbox/left.obj");
			std::shared_ptr<TriangleMesh> mesh =
				std::make_shared<TriangleMesh>(identity, identity,
					info->numbers, info->vertices, info->indices,
					info->normals, info->tangants, info->uvs);
			meshes.push_back(mesh);
			const auto& triangles = mesh->triangles;
			//material of left wall
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.63f, 0.065f, 0.05f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		}

		//floor
		{
			//load mesh
			//std::optional<TriangleInfo> info =
			//	loader.load(PATH + "/models/cornellbox/", "cbox_floor.obj");
			std::optional<TriangleInfo> info =
				loader.load("/models/cornellbox/floor.obj");
			std::shared_ptr<TriangleMesh> mesh =
				std::make_shared<TriangleMesh>(identity, identity,
					info->numbers, info->vertices, info->indices,
					info->normals, info->tangants, info->uvs);
			meshes.push_back(mesh);
			const auto& triangles = mesh->triangles;

			//material of floor
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.725f, 0.71f, 0.68f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		}

		////back
		//{
		//	//load mesh
		//	std::optional<TriangleInfo> info =
		//		loader.load(PATH + "/models/cornellbox/", "cbox_back.obj");
		//	std::shared_ptr<TriangleMesh> mesh =
		//		std::make_shared<TriangleMesh>(identity, identity,
		//			info->numbers, info->vertices, info->indices,
		//			info->normals, info->tangants, info->uvs);
		//	meshes.push_back(mesh);
		//	const auto& triangles = mesh->triangles;
		//
		//	//material of floor
		//	const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.725f, 0.71f, 0.68f));
		//
		//	//generate primitive
		//	for (size_t i = 0; i < triangles.size(); i++)
		//		prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		//}
		//
		////ceiling
		//{
		//	//load mesh
		//	std::optional<TriangleInfo> info =
		//		loader.load(PATH + "/models/cornellbox/", "cbox_ceiling.obj");
		//	std::shared_ptr<TriangleMesh> mesh =
		//		std::make_shared<TriangleMesh>(identity, identity,
		//			info->numbers, info->vertices, info->indices,
		//			info->normals, info->tangants, info->uvs);
		//	meshes.push_back(mesh);
		//	const auto& triangles = mesh->triangles;
		//
		//	//material of floor
		//	const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.725f, 0.71f, 0.68f));
		//
		//	//generate primitive
		//	for (size_t i = 0; i < triangles.size(); i++)
		//		prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		//}

		//tall box
		{
			//load mesh
			//std::optional<TriangleInfo> info =
			//	loader.load(PATH + "/models/cornellbox/", "cbox_largebox.obj");
			std::optional<TriangleInfo> info =
				loader.load("/models/cornellbox/tallbox.obj");
			std::shared_ptr<TriangleMesh> mesh =
				std::make_shared<TriangleMesh>(identity, identity,
					info->numbers, info->vertices, info->indices,
					info->normals, info->tangants, info->uvs);
			meshes.push_back(mesh);
			const auto& triangles = mesh->triangles;

			//material of short box
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.725f, 0.71f, 0.68f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));
		}

		//short box
		{
			//load mesh
			//std::optional<TriangleInfo> info =
			//	loader.load(PATH + "/models/cornellbox/", "cbox_smallbox.obj");
			std::optional<TriangleInfo> info =
				loader.load("/models/cornellbox/shortbox.obj");
			std::shared_ptr<TriangleMesh> mesh =
				std::make_shared<TriangleMesh>(identity, identity,
					info->numbers, info->vertices, info->indices,
					info->normals, info->tangants, info->uvs);
			meshes.push_back(mesh);
			const auto& triangles = mesh->triangles;
			//material of short box
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.725f, 0.71f, 0.68f));

			//generate primitive
			for (size_t i = 0; i < triangles.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles[i], mate));

		}

		//Light
		{
			//load light mesh
			//std::optional<TriangleInfo> info =
			//	loader.load(PATH + "/models/cornellbox/", "cbox_luminaire.obj");
			std::optional<TriangleInfo> info =
				loader.load("/models/cornellbox/light.obj");
			std::shared_ptr<TriangleMesh> mesh =
				std::make_shared<TriangleMesh>(identity, identity,
					info->numbers, info->vertices, info->indices,
					info->normals, info->tangants, info->uvs);
			meshes.push_back(mesh);
			const auto& triangles = mesh->triangles;

			//light material
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.65f));

			//light emittion
			Spectrum lightEmit = RGBSpectrum::fromRGB(8.0 * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f)
				+ 15.6 * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4 *
				Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f));

			//generate light primitive
			for (size_t i = 0; i < triangles.size(); i++) {
				auto l = std::make_shared<DiffuseAreaLight>(identity, identity, 5, triangles[i], lightEmit);
				auto p = std::make_shared<Primitive>(triangles[i], mate, l);
				lights.push_back(l);//adding light ptr to scene instance
				prim_ptrs.push_back(p);//adding pritive 
			}
		}
		return Scene(usedTransform, lights, meshes, prim_ptrs, areal, AccelType::BVH);

	}

	Scene Scene::buildTestSphere() {
		std::vector<std::shared_ptr<TriangleMesh>> meshes;
		std::vector<std::shared_ptr<Transform>> usedTransform;
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<Primitive>> prim_ptrs;
		std::vector<std::shared_ptr<InfiniteAreaLight>> areal;

		std::shared_ptr<Transform> identity = std::make_shared<Transform>(Identity());
		usedTransform.push_back(identity);

		//sphere
		{
			std::shared_ptr<Transform> sphereWorld = std::make_shared<Transform>(Translate(Vector3f(278, 200, 400)));
			std::shared_ptr<Transform> invSW = std::make_shared<Transform>(sphereWorld->inverse());
			usedTransform.push_back(sphereWorld);
			usedTransform.push_back(invSW);

			const auto shape = std::make_shared<Sphere>(sphereWorld, invSW, 190);
			auto mapping = std::make_shared<UVMapping2D>();
			std::shared_ptr<ImageTexture<RGBSpectrum, Spectrum>> kd = std::make_shared<ImageTexture<RGBSpectrum, Spectrum>>
				("C:/Users/HJW/Desktop/earthmap.jpg", mapping, true, ImageWrap::ImClamp, true);

			std::shared_ptr<Texture<double>> sigma = std::make_shared<ConstTexture<double>>(0.0);
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

			std::shared_ptr<TriangleMesh> mesh = CreatePlane(identity, identity,
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

			std::shared_ptr<TriangleMesh> lightMesh = CreatePlane(identity, identity,
				pl0, pl1, pl2, pl3, Normal3f(0.0, -1.0, 0.0));
			meshes.push_back(lightMesh);
			auto shapes = lightMesh->triangles;
			const Spectrum lightEmit = RGBSpectrum::fromRGB(47.0, 47.0, 47.0);
			const auto material = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.5));

			for (size_t i = 0; i < shapes.size(); i++) {
				auto l = std::make_shared<DiffuseAreaLight>(identity, identity, 5, shapes[i], lightEmit);
				auto p = std::make_shared<Primitive>(shapes[i], material, l);
				lights.push_back(l);
				prim_ptrs.push_back(p);
			}
		}

		return Scene(usedTransform, lights, meshes, prim_ptrs, areal, AccelType::BVH);
	}

	Scene Scene::buildTestScene() {
		std::vector<std::shared_ptr<TriangleMesh>> meshes;
		std::vector<std::shared_ptr<Transform>> usedTransform;
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<Primitive>> prim_ptrs;
		std::vector<std::shared_ptr<InfiniteAreaLight>> infAreaLights;;
		std::shared_ptr<Transform> identity = std::make_shared<Transform>(Identity());
		usedTransform.push_back(identity);

		Loader loader;
		//material ball
		{
			Mat4f m1{ 0.482906, 0, 0, 0.0571719,
				0, 0.482906, 0, 0.213656,
				0, 0, 0.482906, 0.0682078,
				0, 0, 0, 1 };
			std::shared_ptr<Transform> m1world = std::make_shared<Transform>(m1);
			std::shared_ptr<Transform> invm1 = std::make_shared<Transform>(m1.inverse());

			Mat4f m2{ 0.482906, 0, 0, 0.156382,
				0, 0.482906, 0, 0.777229,
				0, 0, 0.482906, 0.161698,
				0, 0, 0, 1 };
			std::shared_ptr<Transform> m2world = std::make_shared<Transform>(m2);
			std::shared_ptr<Transform> invm2 = std::make_shared<Transform>(m2.inverse());

			Mat4f m0{ 0.482906, 0, 0, 0.110507,
				0, 0.482906, 0, 0.494301,
				0, 0, 0.482906, 0.126194,
				0, 0, 0, 1 };
			std::shared_ptr<Transform> m0world = std::make_shared<Transform>(m0);
			std::shared_ptr<Transform> invm0 = std::make_shared<Transform>(m0.inverse());
			usedTransform.push_back(m1world);
			usedTransform.push_back(invm1);
			usedTransform.push_back(m2world);
			usedTransform.push_back(invm2);
			usedTransform.push_back(m0world);
			usedTransform.push_back(invm0);

			std::optional<TriangleInfo> mateballInfo =
				loader.load("/models/mateball/meshes/Mesh001.obj");
			std::shared_ptr<TriangleMesh> mesh0 =
				std::make_shared<TriangleMesh>(m0world, invm0,
					mateballInfo->numbers, mateballInfo->vertices, mateballInfo->indices,
					mateballInfo->normals, mateballInfo->tangants, mateballInfo->uvs);
			meshes.push_back(mesh0);
			const auto& triangles0 = mesh0->triangles;

			mateballInfo = loader.load("/models/mateball/meshes/Mesh002.obj");
			std::shared_ptr<TriangleMesh> mesh1 =
				std::make_shared<TriangleMesh>(m1world, invm1,
					mateballInfo->numbers, mateballInfo->vertices, mateballInfo->indices,
					mateballInfo->normals, mateballInfo->tangants, mateballInfo->uvs);
			meshes.push_back(mesh1);
			const auto& triangles1 = mesh1->triangles;

			mateballInfo = loader.load("/models/mateball/meshes/Mesh000.obj");

			std::shared_ptr<TriangleMesh> mesh2 =
				std::make_shared<TriangleMesh>(m2world, invm2,
					mateballInfo->numbers, mateballInfo->vertices, mateballInfo->indices,
					mateballInfo->normals, mateballInfo->tangants, mateballInfo->uvs);
			meshes.push_back(mesh2);
			const auto& triangles2 = mesh2->triangles;

			//textures
			const auto sigma = std::make_shared<ConstTexture<double>>(0.03);
			const auto kd = std::make_shared<ConstTexture<Spectrum>>(Spectrum::fromRGB(0.4, 0.4, 0.4));
			const auto ks = std::make_shared<ConstTexture<Spectrum>>(Spectrum::fromRGB(0.4, 0.4, 0.4));

			//material of right wall
			const auto mate = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.75f));
			const auto mate2 = std::make_shared<Plastic>(kd, ks, sigma);

			//generate primitive
			for (size_t i = 0; i < triangles0.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles0[i], mate));
			for (size_t i = 0; i < triangles1.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles1[i], mate2));
			for (size_t i = 0; i < triangles2.size(); i++)
				prim_ptrs.push_back(std::make_shared<Primitive>(triangles2[i], mate));
		}
		//floor
		{
			Point3f p0(-10.0, 0.0, 10.0);
			Point3f p1(10.0, 0.0, 10.0);
			Point3f p2(10.0, 0.0, -10.0);
			Point3f p3(-10.0, 0.0, -10.0);

			std::shared_ptr<TriangleMesh> mesh = CreatePlane(identity, identity,
				p0, p1, p2, p3, Normal3f(0.0, 1.0, 0.0));
			meshes.push_back(mesh);
			const auto shapes = mesh->triangles;
			const auto material = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.5));

			for (size_t i = 0; i < shapes.size(); i++) {
				auto p = std::make_shared<Primitive>(shapes[i], material);
				prim_ptrs.push_back(p);
			}
		}

		//sphere
		{
			Mat4f m2{ 0.482906, 0, 0, 0,
				0, 0.482906, 0, 2,
				0, 0, 0.482906, 0,
				0, 0, 0, 1 };
			std::shared_ptr<Transform> m2world = std::make_shared<Transform>(m2);
			std::shared_ptr<Transform> invm2 = std::make_shared<Transform>(m2.inverse());
			usedTransform.push_back(m2world);
			usedTransform.push_back(invm2);
			const auto& sphere = std::make_shared<Sphere>(m2world, invm2, 1);

			//textures
			const auto sigma = std::make_shared<ConstTexture<double>>(0.6);
			const auto kd = std::make_shared<ConstTexture<Spectrum>>(Spectrum::fromRGB(0.75));
			const auto ks = std::make_shared<ConstTexture<Spectrum>>(Spectrum::fromRGB(0.9));

			//material
			const auto mate = std::make_shared<Plastic>(kd, ks, sigma);
			//const auto mate = MatteMaterial::buildConst(0,RGBSpectrum(0.75));

			prim_ptrs.push_back(std::make_shared<Primitive>(sphere, mate));
		}

		//light
		{
			Point3f pl0(443.0, 848.7, 127.0);
			Point3f pl1(443.0, 848.7, 232.0);
			Point3f pl2(313.0, 848.7, 232.0);
			Point3f pl3(313.0, 848.7, 127.0);

			std::shared_ptr<TriangleMesh> lightMesh = CreatePlane(identity, identity,
				pl0, pl1, pl2, pl3, Normal3f(0.0, -1.0, 0.0));
			meshes.push_back(lightMesh);
			auto shapes = lightMesh->triangles;
			const Spectrum lightEmit = RGBSpectrum::fromRGB(47.0, 47.0, 47.0);
			const auto material = MatteMaterial::buildConst(0.0, RGBSpectrum::fromRGB(0.5));

			for (size_t i = 0; i < shapes.size(); i++) {
				auto l = std::make_shared<DiffuseAreaLight>(identity, identity, 5, shapes[i], lightEmit);
				auto p = std::make_shared<Primitive>(shapes[i], material, l);
				lights.push_back(l);
				prim_ptrs.push_back(p);
			}
		}

		//light
		{
			Mat4f m{ -0.386527, 0, 0.922278, 0,
				-0.922278, 0, -0.386527, 0,
				0, 1, 0, 0,
				0, 0, 0, 1 };
			std::shared_ptr<Transform> lightToWorld = std::make_shared<Transform>(m);
			std::shared_ptr<Transform>worldToLight = std::make_shared<Transform>(m.inverse());
			usedTransform.push_back(lightToWorld);
			usedTransform.push_back(worldToLight);
			Spectrum lightScale(1.0);
			std::string path("D:/MyWorks/Raven/models/material-testball/material-testball/textures/evnmap.hdr");
			std::shared_ptr<InfiniteAreaLight> light =
				std::make_shared<InfiniteAreaLight>(lightToWorld, worldToLight, lightScale, 2, path);
			lights.push_back(light);
			infAreaLights.push_back(light);
		}
		return Scene(usedTransform, lights, meshes, prim_ptrs, infAreaLights, AccelType::BVH);
	}
}