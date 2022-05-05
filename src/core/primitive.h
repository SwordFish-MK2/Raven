#ifndef _RAVEN_CORE_PRIMITIVE_H_
#define _RAVEN_CORE_PRIMITIVE_H_

#include"base.h"
#include"material.h"
#include"shape.h"


namespace Raven {

	class Primitive {
	public:

		Primitive(const std::shared_ptr<Shape>& shape_ptr, const std::shared_ptr<Material>& mate_ptr,
			const std::shared_ptr<Light>& light = nullptr)
			:shape_ptr(shape_ptr), mate_ptr(mate_ptr), light_ptr(light) {}

		~Primitive() {}

		virtual bool hit(const Ray& r_in, double tMax = FLT_MAX)const;

		virtual bool intersect(const Ray& r_in, HitInfo& inter, double tMax = std::numeric_limits<double>::max())const;

		virtual SurfaceInteraction setInteractionProperty(const HitInfo& p, const RayDifferential& ray)const;

		virtual Bound3f worldBounds()const;

		const Material* getMaterial()const { return mate_ptr.get(); }

		const Light* getAreaLight()const { return light_ptr.get(); }

		const Shape* getShape()const { return shape_ptr.get(); }

		static std::shared_ptr<Primitive> build(const std::shared_ptr<Shape>& s, const std::shared_ptr<Material>& m,
			const std::shared_ptr<Light>& l = nullptr);

		void setMaterial(const std::shared_ptr<Material>& mate) { mate_ptr = mate; }
	private:
		std::shared_ptr<Shape> shape_ptr;
		std::shared_ptr<Material> mate_ptr;
		std::shared_ptr<Light> light_ptr;
	};

	class TransformedPrimitive :public Primitive {
	public:
		TransformedPrimitive(const Transform* ptw, const Transform* wtp, const std::shared_ptr<Primitive> p)
			:Primitive(NULL, NULL), primToWorld(ptw), worldToPrim(wtp), prim(p) {}

		~TransformedPrimitive() {}

		bool hit(const Ray& r_in, double tMax = FLT_MAX)const;

		virtual SurfaceInteraction setInteractionProperty(const HitInfo& p,const RayDifferential& ray) const override;

		Bound3f worldBounds()const;

		static std::shared_ptr<TransformedPrimitive> build(
			const Transform* ptw,
			const Transform* wtp,
			const std::shared_ptr<Primitive>& prim);

	private:

		const Transform* primToWorld;
		const Transform* worldToPrim;
		const std::shared_ptr<Primitive> prim;
	};

}

#endif