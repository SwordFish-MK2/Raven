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
		//Texture<Spectrum>* kd1 = new ConstTexture<Spectrum>(Spectrum::fromRGB(rgb1));
		//Texture<Spectrum>* kd2 = new ConstTexture<Spectrum>(Spectrum::fromRGB(rgb2));
		Texture<double>* sigma1 = new ConstTexture<double>(0.2);
		Texture<double>* sigma2 = new ConstTexture<double>(0.0);
		Texture<Vector3f>* kd1 = new ConstTexture<Vector3f>(Vector3f(0.1, 0.97, 0.4));
		Texture<Vector3f>* kd2 = new ConstTexture<Vector3f>(Vector3f(0.5, 0.5, 0.5));
		//	Texture<double>* sphereTexture = new ConstTexture<double>(1.);

		/*	vectorTexture.push_back(kd1);
			vectorTexture.push_back(kd2);*/
		floatTexture.push_back(sigma1);
		floatTexture.push_back(sigma2);
		//	floatTexture.push_back(sphereTexture);

		MatteMaterial* mate1 = new MatteMaterial(sigma2, kd1, nullptr);
		MatteMaterial* mate2 = new MatteMaterial(sigma2, kd2, nullptr);
		//PureSpecular* specular1 = new PureSpecular(kd1);
		//PureSpecular* specular2 = new PureSpecular(kd2);
		//	PureSpecular* mate3 = new PureSpecular(kd1);
		Sphere* s = new Sphere(sphereLocToPrim, spherePrimToLoc, 80.0, 80.0, -80.0, 2 * M_PI);
		Sphere* ground = new Sphere(groundPrimToWorld, groundWorldToPrim, 16000., 16000., -16000., 2 * M_PI);

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
		n.push_back(Normal3f(0.0, 0.0, -1.0));
		n.push_back(Normal3f(0.0, 0.0, -1.0));

		Eigen::Matrix4f I = Eigen::Matrix4f::Identity();
		Transform* identity = new Transform(I);
		Transform* invI = new Transform(I);
		TriangleMesh* square = new TriangleMesh(identity, invI, 2, v, i, n, tan, uv);

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
		for (int i = 0; i < floatTexture.size(); i++)
			if (floatTexture[i])
				delete floatTexture[i];
		for (int i = 0; i < vectorTexture.size(); i++)
			if (vectorTexture[i])
				delete vectorTexture[i];
		for (int i = 0; i < usedMaterial.size(); i++)
			if (usedMaterial[i])
				delete usedMaterial[i];
	}
}