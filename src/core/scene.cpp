#include"scene.h"
#include"../material/matte.h"
#include"../shape/sphere.h"
#include"../shape/mesh.h"

namespace Raven {
	void Scene::init() {

		//inite transform matrix
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
		double rgb1[3] = { 0.1,0.7,0.4 };
		double rgb2[3] = { 0.5,0.5,0.5 };
		//texture
		std::shared_ptr<Texture<double>> sigma1 = std::make_shared<ConstTexture<double>>(0.2);
		std::shared_ptr<Texture<double>> sigma2 = std::make_shared<ConstTexture<double>>(0.0);
		std::shared_ptr<Texture<Vector3f>> kd1 = std::make_shared<ConstTexture<Vector3f>>(Vector3f(0.1, 0.97, 0.4));
		std::shared_ptr<Texture<Vector3f>> kd2 = std::make_shared<ConstTexture<Vector3f>>(Vector3f(0.5, 0.5, 0.5));
		//	Texture<double>* sphereTexture = new ConstTexture<double>(1.);
		//	floatTexture.push_back(sphereTexture);

		//material
		std::shared_ptr<MatteMaterial> mate1 = std::make_shared<MatteMaterial>(sigma2, kd1, nullptr);
		std::shared_ptr<MatteMaterial> mate2 = std::make_shared<MatteMaterial>(sigma2, kd2, nullptr);
		
		//shape
		std::shared_ptr<Sphere> s = std::make_shared<Sphere>(sphereLocToPrim, spherePrimToLoc, 80.0, 80.0, -80.0, 2 * M_PI);
		std::shared_ptr<Sphere> ground = std::make_shared<Sphere>(groundPrimToWorld, groundWorldToPrim, 16000., 16000., -16000., 2 * M_PI);

		//primitive
		std::shared_ptr<Primitive> s1 = std::make_shared<Primitive>(s, mate2);//small sphere
		std::shared_ptr<Primitive> s2 = std::make_shared<Primitive>(ground, mate2);//ground
		std::shared_ptr<Primitive> sLeft =
			std::make_shared<TransformedPrimitive>(leftPrimToWorld, leftWorldToPrim, s1);
		std::shared_ptr<Primitive> sRight =
			std::make_shared<TransformedPrimitive>(rightPrimToWorld, rightWorldToPrim, s1);
		std::shared_ptr<Primitive> sMiddle =
			std::make_shared<TransformedPrimitive>(middlePrimToWorld, middleWorldToPrim, s1);

		std::vector<Point3f> v;
		std::vector<int> i;
		std::vector<Normal3f> n;
		std::vector<Vector3f> tan;
		std::vector<Point2f> uv;

		v.push_back(Point3f(-80.0, 100.0, 50.0));
		v.push_back(Point3f(80.0, 100.0, 50.0));
		v.push_back(Point3f(80.0, -60.0, 50.0));
		v.push_back(Point3f(-80.0, -60.0, 50.0));

		i.push_back(0);
		i.push_back(1);
		i.push_back(3);
		i.push_back(1);
		i.push_back(2);
		i.push_back(3);

		n.push_back(Normal3f(0.0, 0.0, -1.0));
		n.push_back(Normal3f(0.0, 0.0, -1.0));
		n.push_back(Normal3f(0.0, 0.0, -1.0));
		n.push_back(Normal3f(0.0, 0.0, -1.0));


		uv.push_back(Point2f(0, 1));
		uv.push_back(Point2f(1, 1));
		uv.push_back(Point2f(1, 0));
		uv.push_back(Point2f(0, 0));

		Eigen::Matrix4f I = Eigen::Matrix4f::Identity();
		Transform* identity = new Transform(I);
		Transform* invI = new Transform(I);
		std::shared_ptr<TriangleMesh> square = std::make_shared<TriangleMesh>(identity, invI, 2, v, i, n, tan, uv);

		usedTransform.push_back(identity);
		std::shared_ptr<Primitive> sq = std::make_shared<Primitive>(square, mate2);
		//prims.push_back(s2);
		//prims.push_back(sMiddle);
		//prims.push_back(sLeft);
		//prims.push_back(sRight);
	//	kdTree = std::make_shared<KdTreeAccel>(prims, -1, 80, 1, 0, 5);

		objs.addPrimitive(s2);
		objs.addPrimitive(sMiddle);
		objs.addPrimitive(sq);
		//	objects.addPrimitive(sLeft);
		//	objects.addPrimitive(sRight);

	}
	void Scene::clear() {
		for (int i = 0; i < usedTransform.size(); i++)
			if (usedTransform[i])
				delete usedTransform[i];
	}
}