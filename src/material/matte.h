#ifndef _RAVEN_MATERIAL_MATTE_H_
#define _RAVEN_MATERIAL_MATTE_H_

#include"../core/base.h"
#include"../core/material.h"
#include"../core/math.h"
#include"../core/texture.h"
#include"../core/distribution.h"
#include"../shading_model/lambertain.h"
#include"../shading_model/oren_nayar.h"
namespace Raven {
	//class MatteMaterial :public Material {
//private:
//	Texture<double>* sigma;//surface roughness value 
//	Texture<Spectrum>* kd;//diffuse reflection value
//	Texture<double>* bump;//bump function
//public:
//	MatteMaterial(Texture<double>* sigma, Texture<Spectrum>* Kd, Texture<double>* bump = NULL) :sigma(sigma), kd(Kd), bump(bump) {}
//	//MatteMaterial(double Kd) :Kd(Kd) {}
//	void computeScarttingFunctions(surfaceIntersection& its) const;
//};

	class MatteMaterial :public Material {
	private:
		std::shared_ptr<Texture<double>> sigma;//surface roughness value 
		std::shared_ptr<Texture<Vector3f>> kd;//diffuse reflection value
		std::shared_ptr<Texture<double>> bump;//bump function
	public:
		MatteMaterial(const std::shared_ptr<Texture<double>>& sigma, const std::shared_ptr<Texture<Vector3f>>& Kd,
			const std::shared_ptr<Texture<double>>& bump = NULL)
			:sigma(sigma), kd(Kd), bump(bump) {}
		//MatteMaterial(double Kd) :Kd(Kd) {}
		void computeScarttingFunctions(SurfaceInteraction& its) const;
	};
}

#endif