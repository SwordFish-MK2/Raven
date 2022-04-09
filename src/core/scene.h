#ifndef _RAVEN_CORE_SCENE_H_
#define _RAVEN_CORE_SCENE_H_


#include"base.h"
#include<vector>
#include"aabb.h"
#include"transform.h"
#include"primitive.h"
#include"texture.h"
#include"accelerate.h"
#include"../shape/mesh.h"


namespace Raven {
	class Scene {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	private:
		std::vector<std::shared_ptr<Transform>> transforms;
		std::vector<std::shared_ptr<TriangleMesh>> meshes;
		std::shared_ptr<Accelerate> objs;

	public:
		std::vector<std::shared_ptr<Light>> lights;
		Scene() {}

		Scene(const std::vector<std::shared_ptr<Transform>>& trans, const std::vector<std::shared_ptr<Light>>& lights,
			const std::vector<std::shared_ptr<TriangleMesh>>& meshes, const std::vector<std::shared_ptr<Primitive>>& prims,
			AccelType type = AccelType::List);

		Scene(const Scene& s);

		//测试光线是否与场景相交
		bool hit(const Ray& r, double tMax)const {
			return objs->hit(r, tMax);
		}
		//判断光线是否与场景相交，如果相交，计算交点信息
		std::optional<SurfaceInteraction> intersect(const Ray& r, double tMax)const {
			return objs->intersect(r, tMax);
		}

		const Light* chooseLight(double rand)const;

		Spectrum sampleLight(const SurfaceInteraction& record, double s, const Point2f& uv, LightSample* sample)const;

		Bound3f worldBound()const {
			return objs->worldBounds();
		}

		static Scene buildTestScene();

		static Scene buildCornellBox();

		static Scene buildTestSphere();
	};
}
#endif