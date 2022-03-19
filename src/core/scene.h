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
		//test if incident ray hit any object in the scene 
		bool hit(const Ray& r,double tMin,double tMax)const {
			return objs->hit(r,tMin,tMax);
		}
		//test if incident ray hit any object in the scene
		//if hit, get ray intersection infomation 
		bool intersect(const Ray& r, SurfaceInteraction& its, double tMin, double tMax)const {
			return objs->intersect(r, its, tMin, tMax);
		}

		Bound3f worldBound()const {
			return objs->worldBounds();
		}

		const Light* chooseLight(double random)const;

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			void init();
		void clear();
	};
}
#endif