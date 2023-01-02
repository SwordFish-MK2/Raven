#ifndef _RAVEN_CORE_SCENE_H_
#define _RAVEN_CORE_SCENE_H_

#include<Raven/core/base.h>
#include<vector>
#include<Raven/core/aabb.h>
#include<Raven/core/transform.h>
#include<Raven/core/primitive.h>
#include<Raven/core/texture.h>
#include<Raven/core/accelerate.h>
#include<Raven/shape/mesh.h>
#include<string>
#include<Raven/core/interaction.h>
#include<Raven/core/object.h>


namespace Raven {
	class Scene :public RavenObject {
	private:
		std::vector<std::shared_ptr<Transform>> transforms;
		std::vector<std::shared_ptr<TriangleMesh>> meshes;
		std::shared_ptr<Accelerate> objs;//场景加速结构，包含所有的Primitive

	public:
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<InfiniteAreaLight>> infinitAreaLights;

		Scene() {}

		Scene(const std::vector<std::shared_ptr<Transform>>& trans, const std::vector<std::shared_ptr<Light>>& lights,
			const std::vector<std::shared_ptr<TriangleMesh>>& meshes, const std::vector<std::shared_ptr<Primitive>>& prims,
			const std::vector<std::shared_ptr<InfiniteAreaLight>>& areal,
			AccelType type = AccelType::List);

		Scene(const Scene& s);

		//测试光线是否与场景相交
		bool hit(const RayDifferential& r, double tMax)const {
			return objs->hit(r, tMax);
		}

		//void addPrimitive(const std::shared_ptr<Primitive>& pri_ptr);

		void addMesh(const std::shared_ptr<TriangleMesh>& mesh_ptr);

		void addTransform(const std::shared_ptr<Transform>& trans_ptr);

		void addLight(const std::shared_ptr<Light>& light_ptr);

		//判断光线是否与场景相交，如果相交，计算交点信息
		std::optional<SurfaceInteraction> intersect(const RayDifferential& r)const {
			return objs->intersect(r);
		}

		//const Light* chooseLight(double rand)const;

		const Bound3f worldBound()const {
			return objs->worldBounds();
		}

		//static Scene cornellBox();

		static Scene buildTestScene();

		static Scene buildCornellBox();

		static Scene buildTestSphere();
	};
}
#endif