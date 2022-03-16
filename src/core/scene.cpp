#include"scene.h"
#include"../material/matte.h"
#include"../shape/sphere.h"
#include"../shape/mesh.h"
#include"../light/areaLight.h"
namespace Raven {
	void Scene::init() {

		//transform
		Eigen::Matrix4f spherePrimitive;
		spherePrimitive <<
			1.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, -1.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 1.f;
		Eigen::Matrix4f sphereGroundModel;
		Eigen::Matrix4f sphereMiddleModel;
		Eigen::Matrix4f sphereLeftModel;
		Eigen::Matrix4f sphereRightModel;
		sphereMiddleModel <<
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 150.f,
			0.f, 0.f, 0.f, 1.f;
		sphereGroundModel <<
			1.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 1.f, -16080.f,
			0.f, -1.f, 0.f, 150.f,
			0.f, 0.f, 0.f, 1.f;
		sphereRightModel <<
			1.f, 0.f, 0.f, 160.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 150.f,
			0.f, 0.f, 0.f, 1.f;
		sphereLeftModel <<
			1.f, 0.f, 0.f, -160.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 150.f,
			0.f, 0.f, 0.f, 1.f;

		Transform* sphereLocToPrim = new Transform(spherePrimitive);//Transform all sphere from local space to primitive space
		Transform* spherePrimToLoc = new Transform(spherePrimitive.inverse());//inverse transform
		usedTransform.push_back(sphereLocToPrim);
		usedTransform.push_back(spherePrimToLoc);

		Transform* groundPrimToWorld = new Transform(sphereGroundModel);//world position of ground sphere 
		Transform* groundWorldToPrim = new Transform(sphereGroundModel.inverse());//inverse transform
		usedTransform.push_back(groundPrimToWorld);
		usedTransform.push_back(groundWorldToPrim);

		Transform* middlePrimToWorld = new Transform(sphereMiddleModel);//world position of sphere in the middle
		Transform* middleWorldToPrim = new Transform(sphereMiddleModel.inverse());
		usedTransform.push_back(middlePrimToWorld);
		usedTransform.push_back(middleWorldToPrim);

		Transform* leftPrimToWorld = new Transform(sphereLeftModel);//world position of sphere in the left
		Transform* leftWorldToPrim = new Transform(sphereLeftModel.inverse());
		usedTransform.push_back(leftPrimToWorld);
		usedTransform.push_back(leftWorldToPrim);

		Transform* rightPrimToWorld = new Transform(sphereRightModel);//world position of sphere int the right
		Transform* rightWorldToPrim = new Transform(sphereRightModel.inverse());
		usedTransform.push_back(rightPrimToWorld);
		usedTransform.push_back(rightWorldToPrim);

		Eigen::Matrix4f I = Eigen::Matrix4f::Identity();
		Transform* identity = new Transform(I);
		usedTransform.push_back(identity);

		double rgb1[3] = { 0.1,0.7,0.4 };
		double rgb2[3] = { 0.5,0.5,0.5 };


		//Texture<Spectrum>* kd1 = new ConstTexture<Spectrum>(Spectrum::fromRGB(rgb1));
		//Texture<Spectrum>* kd2 = new ConstTexture<Spectrum>(Spectrum::fromRGB(rgb2));
		//texture
		std::shared_ptr<Texture<double>> sigma1 = std::make_shared<ConstTexture<double>>(0.2);
		std::shared_ptr<Texture<double>> sigma2 = std::make_shared<ConstTexture<double>>(0.0);
		std::shared_ptr<Texture<Vector3f>> kd1 = std::make_shared<ConstTexture<Vector3f>>(Vector3f(0.1, 0.97, 0.4));
		std::shared_ptr<Texture<Vector3f>> kd2 = std::make_shared<ConstTexture<Vector3f>>(Vector3f(0.5, 0.5, 0.5));

		//material
		std::shared_ptr<MatteMaterial> mate1 = std::make_shared<MatteMaterial>(sigma2, kd1, nullptr);
		std::shared_ptr<MatteMaterial> mate2 = std::make_shared<MatteMaterial>(sigma2, kd2, nullptr);

		//shape
		std::shared_ptr<Sphere> s = std::make_shared<Sphere>(sphereLocToPrim, spherePrimToLoc, 80.0, 80.0, -80.0, 2 * M_PI);
		std::shared_ptr<Sphere> ground = std::make_shared<Sphere>(groundPrimToWorld, groundWorldToPrim, 16000., 16000., -16000., 2 * M_PI);


		Point3f p0(-500.0, -80.0, 700.0);
		Point3f p1(500.0, -80.0, 700.0);
		Point3f p2(500.0, -80.0, -300.0);
		Point3f p3(-500.0, -80.0, -300.0);

		Point3f sp0(-50.0, 500.0, 200.0);
		Point3f sp1(50.0, 500.0, 200.0);
		Point3f sp2(50.0, 500.0, 100.0);
		Point3f sp3(-50.0, 500.0, 100.0);


		//Point3f sp0(200.0, 5.0, 155.0);
		//Point3f sp1(200.0, 5.0, 145.0);
		//Point3f sp2(200.0, -5.0, 145.0);
		//Point3f sp3(200.0, -5.0, 155.0);

		//Point3f p0(-80.0, 100.0, 200.0);
		//Point3f p1(80.0, 100.0, 200.0);
		//Point3f p2(80.0, -60.0, 200.0);
		//Point3f p3(-80.0, -60.0, 200.0);
		std::shared_ptr<TriangleMesh> sqr = std::make_shared<TriangleMesh>(CreatePlane(identity, identity,
			sp0, sp1, sp2, sp3, Normal3f(0.0, -1.0, 0.0)));
		std::vector<std::shared_ptr<Triangle>> sTri = sqr->getTriangles();
		meshes.push_back(sqr);

		std::shared_ptr<DiffuseAreaLight> aLight1 =
			std::make_shared<DiffuseAreaLight>(identity, identity, 5, sTri[0].get(), Vector3f(2.0, 2.0, 2.0));
		std::shared_ptr<DiffuseAreaLight> aLight2 =
			std::make_shared<DiffuseAreaLight>(identity, identity, 5, sTri[1].get(), Vector3f(2.0, 2.0, 2.0));
		std::shared_ptr<Primitive> lightp1 = std::make_shared<Primitive>(sTri[0], nullptr, aLight1);
		std::shared_ptr<Primitive> lightp2 = std::make_shared<Primitive>(sTri[1], nullptr, aLight2);
		lights.push_back(aLight1);
		lights.push_back(aLight2);
		std::shared_ptr<TriangleMesh> square = std::make_shared<TriangleMesh>(CreatePlane(identity, identity,
			p0, p1, p2, p3, Normal3f(0.0, 1.0, 0.0)));
		std::vector<std::shared_ptr<Primitive>> squareTri = square->generatePrimitive(mate2);
		meshes.push_back(square);
		//primitive
		std::shared_ptr<Primitive> s1 = std::make_shared<Primitive>(s, mate2);//small sphere
		std::shared_ptr<Primitive> s2 = std::make_shared<Primitive>(ground, mate2);//ground
		std::shared_ptr<Primitive> sLeft =
			std::make_shared<TransformedPrimitive>(leftPrimToWorld, leftWorldToPrim, s1);
		std::shared_ptr<Primitive> sRight =
			std::make_shared<TransformedPrimitive>(rightPrimToWorld, rightWorldToPrim, s1);
		std::shared_ptr<Primitive> sMiddle =
			std::make_shared<TransformedPrimitive>(middlePrimToWorld, middleWorldToPrim, s1);
		//	std::shared_ptr<Primitive> sq = std::make_shared<Primitive>(square, mate2);

		std::vector<std::shared_ptr<Primitive>> prim_ptrs;
		//prim_ptrs.push_back(sMiddle);
		//prim_ptrs.push_back(s2);
		prim_ptrs.push_back(lightp1);
		prim_ptrs.push_back(lightp2);
		prim_ptrs.push_back(squareTri[0]);
		prim_ptrs.push_back(squareTri[1]);
		prim_ptrs.push_back(sMiddle);

		//prim_ptrs.push_back(sq);
		squareTri.push_back(sMiddle);
		objs = std::make_shared<PrimitiveList>(prim_ptrs);

	}

	void Scene::clear() {
		for (int i = 0; i < usedTransform.size(); i++)
			if (usedTransform[i])
				delete usedTransform[i];

	}

	const Light* Scene::chooseLight(double random)const {
		Vector3f totalPower(0.0);
		for (int i = 0; i < lights.size(); i++) {
			totalPower += lights[i]->power();
		}
		Vector3f power(0.0);
		for (int i = 0; i < lights.size(); i++) {
			power += lights[i]->power();
			double temp = power.y / totalPower.y;
			if (temp >= random || i == lights.size() - 1)
				return lights[i].get();
		}
	}
}