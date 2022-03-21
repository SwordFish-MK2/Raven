#ifndef _RAVEN_CORE_SCENE_H_
#define _RAVEN_CORE_SCENE_H_


#include"base.h"
#include<vector>
#include"aabb.h"
#include"transform.h"
#include"primitive.h"
#include"texture.h"
#include"accelerate.h"
#include"../accelerate/primitiveList.h"
#include"../shape/mesh.h"


namespace Raven {
	class Scene {
	private:
		std::vector<std::shared_ptr<Transform>> usedTransform;
		std::vector<std::shared_ptr<TriangleMesh>> meshes;
		std::shared_ptr<Accelerate> objs;
		std::vector<std::shared_ptr<Light>> lights;
	public:

		//测试光线是否与场景相交
		bool hit(const Ray& r, double tMin, double tMax)const {
			return objs->hit(r, tMin, tMax);
		}
		//判断光线是否与场景相交，如果相交，计算交点信息
		bool intersect(const Ray& r, SurfaceInteraction& its, double tMin, double tMax)const {
			return objs->intersect(r, its, tMin, tMax);
		}

		Vector3f sampleLight(const SurfaceInteraction& record, double s, const Point2f& uv, LightSample* sample)const;

		Bound3f worldBound()const {
			return objs->worldBounds();
		}

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			void buildCornellBox();
	};
}
#endif