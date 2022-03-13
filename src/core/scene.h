#ifndef _RAVEN_CORE_SCENE_H_
#define _RAVEN_CORE_SCENE_H_


#include"base.h"
#include<vector>
#include"aabb.h"
#include"transform.h"
#include"primitive.h"
#include"texture.h"

namespace Raven {
	class Scene {
	private:
		std::vector<Transform*> usedTransform;
		//std::vector<std::shared_ptr<Primitive>> prims;
		PrimitiveList objs;

	public:
		//std::vector<std::shared_ptr<Light>> lights;
		//test if incident ray hit any object in the scene 
		bool hit(const Ray& r)const {
			return objs.hit(r);
		}
		//test if incident ray hit any object in the scene
		//if hit, get ray intersection infomation 
		bool intersect(const Ray& r, SurfaceInteraction& its, double tMin, double tMax)const {
			return objs.intersect(r, its, tMin, tMax);
			//return kdTree->intersect(r, its, tMin, tMax);
		}

		Bound3f worldBound()const {
			return objs.worldBounds();
		}

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			void init();
		void clear();
	};
}
#endif