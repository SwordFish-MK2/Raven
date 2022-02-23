#ifndef _RAVEN_CORE_PRIMITIVE_H_
#define _RAVEN_CORE_PRIMITIVE_H_

#include"base.h"
#include"material.h"
#include"shape.h"


namespace Raven {
	class Primitive {
	public:
		Primitive(const Shape* shape_ptr,const Material* mate_ptr,const Light* light = nullptr)
			:shape_ptr(shape_ptr), mate_ptr(mate_ptr), light_ptr(light) {}
		~Primitive() {}
		virtual bool hit(const Ray& r_in, double tMin = 0.001, double tMax = FLT_MAX)const;
		virtual bool intersect(const Ray& r_in, SurfaceInteraction& its, double tMin = 0.001, double tMax = FLT_MAX)const;
		virtual Bound3f worldBounds()const;
		virtual const Material* getMaterial()const { return mate_ptr; }
		virtual const Light* getAreaLight()const { return light_ptr; }
		virtual const Shape* getShape()const { return shape_ptr; }
	private:
		const Shape* shape_ptr;
		const Material* mate_ptr;
		const Light* light_ptr;
	};

	class TransformedPrimitive :public Primitive {
	public:
		TransformedPrimitive(const Transform* ptw, const Transform* wtp, const std::shared_ptr<Primitive> p)
			:Primitive(NULL, NULL), primToWorld(ptw), worldToPrim(wtp), prim(p) {}
		~TransformedPrimitive() {}
		virtual bool hit(const Ray& r_in, double tMin = 0.001F, double tMax = FLT_MAX)const;
		virtual bool intersect(const Ray& r_in, SurfaceInteraction& its, double tMin = 0.001F, double tMax = FLT_MAX)const;
		virtual Bound3f worldBounds()const;
	private:
		const Transform* primToWorld;
		const Transform* worldToPrim;
		const std::shared_ptr<Primitive> prim;
	};

	class PrimitiveList :public Primitive {
	public:
		PrimitiveList() :Primitive(NULL, NULL) {}
		virtual bool hit(const Ray& r_in, double tMin = 0.001, double tMax = FLT_MAX)const;
		virtual bool intersect(const Ray& r_in, SurfaceInteraction& its, double tMin = 0.001, double tMax = FLT_MAX)const;
		virtual Bound3f worldBounds()const;
		void addPrimitive(std::shared_ptr<Primitive> pri) {
			if (pri)
				pris.push_back(pri);
		}
	private:
		std::vector<std::shared_ptr<Primitive>> pris;
	};
}

#endif