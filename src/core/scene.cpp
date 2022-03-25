#include"scene.h"
#include"../material/matte.h"
#include"../shape/sphere.h"
#include"../shape/mesh.h"
#include"../light/areaLight.h"
#include"../utils/loader.h"
#include"../texture/solidTexture.h"
namespace Raven {

	Scene::Scene(const std::vector<std::shared_ptr<Transform>>& trans, const std::vector<std::shared_ptr<Light>>& lights,
		const std::vector<std::shared_ptr<TriangleMesh>>& meshes, const std::vector<std::shared_ptr<Primitive>>& prims,
		AccelType type) :
		meshes(meshes), lights(lights), transforms(trans) {
		if (type == AccelType::List) {
			objs = std::make_shared<PrimitiveList>(prims);
		}
		else if (type == AccelType::KdTree) {
			objs = std::make_shared<KdTreeAccel>(prims, 10, 80, 1, 0.5, 5);
		}
	}

	Scene::Scene(const Scene& s) :transforms(s.transforms), lights(s.lights), meshes(s.meshes), objs(s.objs) {}

	const Light* Scene::chooseLight(double rand) const{
		Vector3f totalPower(0.0);
		const Light* light;
		for (int i = 0; i < lights.size(); i++) {
			totalPower += lights[i]->power();
		}
		Vector3f power(0.0);
		double p = 1.0;
		for (int i = 0; i < lights.size(); i++) {
			power += lights[i]->power();
			p = power.y / totalPower.y;
			if (p >= rand || i == lights.size() - 1) {
				return lights[i].get();
			}
		}
	}

	Scene Scene::buildCornellBox() {
		std::vector<std::shared_ptr<TriangleMesh>> meshes;
		std::vector<std::shared_ptr<Transform>> usedTransform;
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<Primitive>> prim_ptrs;
		//transform
		//Eigen::Matrix4f spherePrimitive;
		//spherePrimitive <<
		//	1.f, 0.f, 0.f, 0.f,
		//	0.f, 0.f, 1.f, 0.f,
		//	0.f, -1.f, 0.f, 0.f,
		//	0.f, 0.f, 0.f, 1.f;
		std::shared_ptr<Transform> identity = std::make_shared<Transform>(Identity());
		usedTransform.push_back(identity);

		//std::shared_ptr<Transform> sphereLocToPrim = std::make_shared<Transform>(spherePrimitive);
		//std::shared_ptr<Transform> spherePrimToLoc = std::make_shared<Transform>(spherePrimitive.inverse());
		//usedTransform.push_back(sphereLocToPrim);
		//usedTransform.push_back(spherePrimToLoc);

		//std::shared_ptr<Transform> groundWorld =
		//	std::make_shared<Transform>(Translate(Vector3f(0.f, -16080.f, 150.f)));
		//std::shared_ptr<Transform> rightWorld =
		//	std::make_shared<Transform>(Translate(Vector3f(160.f, 0.f, 150.f)));
		//std::shared_ptr<Transform> leftWorld =
		//	std::make_shared<Transform>(Translate(Vector3f(-160.f, 0.f, 150.f)));
		//std::shared_ptr<Transform> middleWorld =
		//	std::make_shared<Transform>(Translate(Vector3f(0.f, 0.f, 150.f)));

		//std::shared_ptr<Transform> groundLocal =
		//	std::make_shared<Transform>(Inverse(*groundWorld));
		//std::shared_ptr<Transform> rightLocal =
		//	std::make_shared<Transform>(Inverse(*rightWorld));
		//std::shared_ptr<Transform> leftLocal =
		//	std::make_shared<Transform>(Inverse(*leftWorld));
		//std::shared_ptr<Transform> middleLocal =
		//	std::make_shared<Transform>(Inverse(*middleWorld));

		//usedTransform.push_back(groundWorld);
		//usedTransform.push_back(rightWorld);
		//usedTransform.push_back(leftWorld);
		//usedTransform.push_back(middleWorld);
		//usedTransform.push_back(groundLocal);
		//usedTransform.push_back(rightLocal);
		//usedTransform.push_back(leftLocal);
		//usedTransform.push_back(middleWorld);

		//Texture
		std::shared_ptr<Texture<Vector3f>> greenTexture = ConstTexture<Vector3f>::build(Vector3f(0.843137, 0.843137, 0.091f));
		std::shared_ptr<Texture<Vector3f>> blackTexture = ConstTexture<Vector3f>::build(Vector3f(0.235294, 0.67451, 0.843137));
		std::shared_ptr<Texture<Vector3f>> cheTex = CheckeredTexture<Vector3f>::build(greenTexture, blackTexture);
		std::shared_ptr<Texture<double>> sigma = ConstTexture<double>::build(0.0);
		//Material
		std::shared_ptr<MatteMaterial> mate1 = MatteMaterial::buildConst(0.0, Vector3f(0.1, 0.97, 0.4));
		std::shared_ptr<MatteMaterial> mate2 = MatteMaterial::buildConst(0.0, Vector3f(0.5, 0.5, 0.5));
		std::shared_ptr<MatteMaterial> redLam = MatteMaterial::buildConst(0.0, Vector3f(0.63f, 0.065f, 0.05f));
		std::shared_ptr<MatteMaterial> greenLam = MatteMaterial::buildConst(0.0, Vector3f(0.14f, 0.45f, 0.091f));
		std::shared_ptr<MatteMaterial> whiteLam = MatteMaterial::buildConst(0.0, Vector3f(0.725f, 0.71f, 0.68f));
		std::shared_ptr<MatteMaterial> lightLam = MatteMaterial::buildConst(0.0, Vector3f(0.65f));
		std::shared_ptr<MatteMaterial> checkered = MatteMaterial::build(sigma, cheTex);

		//Shape
		//std::shared_ptr<Sphere> s = std::make_shared<Sphere>(sphereLocToPrim.get(), spherePrimToLoc.get(), 80.0, 80.0, -80.0, 2 * M_PI);
		//std::shared_ptr<Sphere> ground = std::make_shared<Sphere>(sphereLocToPrim.get(), spherePrimToLoc.get(), 16000., 16000., -16000., 2 * M_PI);

		Loader loader;
		std::optional<TriangleInfo> leftInfo =
			loader.loadObj("D:/MyWorks/Raven/models/cornellbox/left.obj");
		std::optional<TriangleInfo> rightInfo = 
			loader.loadObj("D:/MyWorks/Raven/models/cornellbox/right.obj");
		std::optional<TriangleInfo> floorInfo = 
			loader.loadObj("D:/MyWorks/Raven/models/cornellbox/floor.obj");
		std::optional<TriangleInfo> sBoxInfo = 
			loader.loadObj("D:/MyWorks/Raven/models/cornellbox/shortbox.obj");
		std::optional<TriangleInfo> tBoxInfo =
			loader.loadObj("D:/MyWorks/Raven/models/cornellbox/tallbox.obj");
		std::optional<TriangleInfo> lightInfo = 
			loader.loadObj("D:/MyWorks/Raven/models/cornellbox/light.obj");

		std::shared_ptr<TriangleMesh> leftMesh =
			TriangleMesh::build(identity.get(), identity.get(), *leftInfo);

		std::shared_ptr<TriangleMesh> rightMesh = 
			TriangleMesh::build(identity.get(), identity.get(), *rightInfo);

		std::shared_ptr<TriangleMesh> floorMesh =
			TriangleMesh::build(identity.get(), identity.get(), *floorInfo);

		std::shared_ptr<TriangleMesh> sBoxMesh =
			TriangleMesh::build(identity.get(), identity.get(), *sBoxInfo);

		std::shared_ptr<TriangleMesh> tBoxMesh =
			TriangleMesh::build(identity.get(), identity.get(), *tBoxInfo);

		std::shared_ptr<TriangleMesh> lightMesh =
			TriangleMesh::build(identity.get(), identity.get(), *lightInfo);

		meshes.push_back(leftMesh);
		meshes.push_back(rightMesh);
		meshes.push_back(floorMesh);
		meshes.push_back(sBoxMesh);
		meshes.push_back(tBoxMesh);
		meshes.push_back(lightMesh);


		//Point3f p0(-500.0, -80.0, 700.0);
		//Point3f p1(500.0, -80.0, 700.0);
		//Point3f p2(500.0, -80.0, -300.0);
		//Point3f p3(-500.0, -80.0, -300.0);

		//Point3f sp0(-50.0, 500.0, 200.0);
		//Point3f sp1(50.0, 500.0, 200.0);
		//Point3f sp2(50.0, 500.0, 100.0);
		//Point3f sp3(-50.0, 500.0, 100.0);

		//std::shared_ptr<TriangleMesh> sqr = std::make_shared<TriangleMesh>(CreatePlane(identity.get(), identity.get(),
		//	sp0, sp1, sp2, sp3, Normal3f(0.0, -1.0, 0.0)));
		std::vector<std::shared_ptr<Triangle>> sTri = lightMesh->getTriangles();
		//meshes.push_back(sqr);

		//Light
		Vector3f lightEmit = (8.0 * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f)
			+ 15.6 * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4 *
			Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f));
		std::shared_ptr<DiffuseAreaLight> aLight1 =
			std::make_shared<DiffuseAreaLight>(identity.get(), identity.get(), 5, sTri[0].get(), lightEmit);
		std::shared_ptr<DiffuseAreaLight> aLight2 =
			std::make_shared<DiffuseAreaLight>(identity.get(), identity.get(), 5, sTri[1].get(), lightEmit);
		std::shared_ptr<Primitive> lightp1 = std::make_shared<Primitive>(sTri[0], lightLam, aLight1);
		std::shared_ptr<Primitive> lightp2 = std::make_shared<Primitive>(sTri[1], lightLam, aLight2);
		lights.push_back(aLight1);
		lights.push_back(aLight2);
		//std::shared_ptr<TriangleMesh> square = std::make_shared<TriangleMesh>(CreatePlane(identity.get(), identity.get(),
		//	p0, p1, p2, p3, Normal3f(0.0, 1.0, 0.0)));
		//std::vector<std::shared_ptr<Primitive>> squareTri = square->generatePrimitive(mate2);
		//meshes.push_back(square);

		//Primitive
		//std::shared_ptr<Primitive> s1 = std::make_shared<Primitive>(s, mate2);//small sphere
		//std::shared_ptr<Primitive> s2 = std::make_shared<Primitive>(ground, mate2);//ground
		//std::shared_ptr<Primitive> sLeft =
		//	std::make_shared<TransformedPrimitive>(leftWorld.get(), leftLocal.get(), s1);
		//std::shared_ptr<Primitive> sRight =
		//	std::make_shared<TransformedPrimitive>(rightWorld.get(), rightLocal.get(), s1);
		//std::shared_ptr<Primitive> sMiddle =
		//	std::make_shared<TransformedPrimitive>(middleWorld.get(), middleLocal.get(), s1);
		//	std::shared_ptr<Primitive> sq = std::make_shared<Primitive>(square, mate2);

		std::vector<std::shared_ptr<Primitive>> leftPrim = leftMesh->generatePrimitive(redLam);
		std::vector<std::shared_ptr<Primitive>>  rightPrim = rightMesh->generatePrimitive(checkered);
		std::vector<std::shared_ptr<Primitive>>  floorPrim = floorMesh->generatePrimitive(whiteLam);
		std::vector<std::shared_ptr<Primitive>>  sBoxPrim = sBoxMesh->generatePrimitive(whiteLam);
		std::vector<std::shared_ptr<Primitive>> tBoxPrim = tBoxMesh->generatePrimitive(whiteLam);

		prim_ptrs.push_back(lightp1);
		prim_ptrs.push_back(lightp2);
		prim_ptrs.insert(prim_ptrs.end(), leftPrim.begin(), leftPrim.end());
		prim_ptrs.insert(prim_ptrs.end(), rightPrim.begin(), rightPrim.end());
		prim_ptrs.insert(prim_ptrs.end(), floorPrim.begin(), floorPrim.end());
		prim_ptrs.insert(prim_ptrs.end(), sBoxPrim.begin(), sBoxPrim.end());
		prim_ptrs.insert(prim_ptrs.end(), tBoxPrim.begin(), tBoxPrim.end());
		//prim_ptrs.push_back(squareTri[0]);
		//prim_ptrs.push_back(squareTri[1]);
		//prim_ptrs.push_back(sMiddle);

		//prim_ptrs.push_back(sq);
		//squareTri.push_back(sMiddle);
		return Scene(usedTransform, lights, meshes, prim_ptrs);

	}

	Vector3f Scene::sampleLight(const SurfaceInteraction& record, double s, const Point2f& uv, LightSample* sample)const {
		Vector3f totalPower(0.0);
		const Light* light;
		for (int i = 0; i < lights.size(); i++) {
			totalPower += lights[i]->power();
		}
		Vector3f power(0.0);
		double p = 1.0;
		for (int i = 0; i < lights.size(); i++) {
			power += lights[i]->power();
			p = power.y / totalPower.y;
			if (p >= s || i == lights.size() - 1) {
				light = lights[i].get();
				break;
			}
		}
		if (light)
			return light->sampleLi(record, uv, sample) / p;
		else
			return Vector3f(0.0);
	}

}