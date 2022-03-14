#ifndef _RAVEN_LIGHT_AREA_LIGHT_H_
#define _RAVEN_LIGHT_AREA_LIGHT_H_


#include"../core/base.h"
#include"../core/light.h"


namespace Raven {

	class AreaLight :public Light {
	public:
		AreaLight(const Transform& LTW, const Transform& WTL, int flag, int nSamples, const Shape* shape) :
			Light(LTW, WTL, flag, nSamples), shape_ptr(shape), area(shape->area()) {}
		//compute light emitted radiance
		//virtual Spectrum L(const surfaceIntersection& p, const vectorf3& wi)const = 0;
		virtual Vector3f L(const SurfaceInteraction& p, const Vector3f& wi)const = 0;
	protected:
		const Shape* shape_ptr;
		const double area;
	};


	/// <summary>
/// Diffuse area light uniformly emit light in all direction from all points on the surface
/// </summary>
	class DiffuseAreaLight :public AreaLight {
	public:
		DiffuseAreaLight(const Transform& LTW, const Transform& WTL, int flag,
			int nSamples, const Shape* shape, Vector3f I) :
			AreaLight(LTW, WTL, flag, nSamples, shape), emittedRadiance(I) {}
	//	virtual Spectrum L(const surfaceIntersection& p, const vectorf3& wi)const;
	//	virtual Spectrum sample_Li(const surfaceIntersection& inter, const pointf2& uv,
	//		vectorf3* wi, double* pdf, VisibilityTester* vTester)const;
	//	virtual Spectrum power()const;
	//private:
	//	const Spectrum emittedRadiance;

		virtual Vector3f L(const SurfaceInteraction& p, const Vector3f& wi)const;
		virtual Vector3f sample_Li(const SurfaceInteraction& inter, const Point2f& uv,
			Vector3f* wi, double* pdf,Point3f* lightSample)const;
		virtual Vector3f power()const;
	private:
		const Vector3f emittedRadiance;
	};
}


#endif 