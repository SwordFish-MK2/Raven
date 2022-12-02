#ifndef _RAVEN_CORE_RENDERER_H_
#define _RAVEN_CORE_RENDERER_H_

#include<Raven/core/base.h>
#include<Raven/core/camera.h>
#include<Raven/core/film.h>
#include<Raven/core/scene.h>
#include<Raven/core/interaction.h>
#include<Raven/core/spectrum.h>
#include<Raven/core/bsdf.h>
#include<Raven/core/object.h>

namespace Raven {
#define _RAVEN_INTEGRATOR_REG_(regName,className,constructor)\
	class className##Reg{\
	private:\
		className##Reg(){\
			IntegratorFactory::regClass(#regName,constructor);\
		}\
		static className##Reg regHelper;\
	};\

	class Integrator :public RavenObject {
	public:
		Integrator(
			int spp = 100,
			double epsilon = 0.0001) : spp(spp), epsilon(epsilon) {}

		//利用传入的Camera渲染场景
		//渲染的结果输出传入的到Film中，
		//传入的camera与film为指针以实现多态
		virtual void render(const Scene&, const Ref<Camera>&, Ref<Film>&)const = 0;

	protected:
		const double epsilon;
		//std::shared_ptr<Camera> camera;
		//std::shared_ptr<Film> film;
		const int spp;
	};

	//计算MIS weight
	inline double PowerHeuristic(int fNum, double fPdf, int gNum, double gPdf) {
		double f = fNum * fPdf;
		double g = gNum * gPdf;
		return f * f / (f * f + g * g);
	}

	Spectrum EvaluateLight(const SurfaceInteraction& record, const Scene& scene, const Light& light);

	Spectrum SampleOneLight(const SurfaceInteraction& record, const Scene& scene, int nSample);

	Spectrum SampleAllLights(const SurfaceInteraction& record, const Scene& scene);


	class IntegratorFactory {
		using IntegratorConstructor = std::function<Ref<Integrator>(const PropertyList&)>;

	public:
		static bool registed(const std::string& className);

		static void regClass(const std::string& className, const IntegratorConstructor& param);

		static Ref<Integrator> generateClass(const std::string& className, const PropertyList& param);

		static std::map<std::string, IntegratorConstructor>& getMap() {
			static std::map<std::string, IntegratorConstructor> mapInstance;
			return mapInstance;
		}
	private:

	};
}

#endif