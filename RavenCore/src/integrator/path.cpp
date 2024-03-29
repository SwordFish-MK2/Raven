#include <Raven/core/light.h>
#include <Raven/core/sampler.h>
#include <Raven/integrator/path.h>
#include <Raven/light/infiniteAreaLight.h>
#include <omp.h>

#include "Raven/core/integrator.h"

namespace Raven {
// 路径追踪算法
Spectrum PathTracingIntegrator::renderPixel(const Scene&           scene,
                                            const RayDifferential& rayIn,
                                            Sampler* threadSampler) const {
  //	Spectrum backgroundColor = Spectrum(Spectrum::fromRGB(0.235294,
  // 0.67451,
  // 0.843137));
  Spectrum        Li(0.0);
  Spectrum        beta(1.0);  // 光线的衰减参数
  RayDifferential ray = rayIn;

  bool   specularBounce = false;
  double etaScale       = 1;
  for (int bounce = 0; bounce < maxDepth; bounce++) {
    // 获取场景与光线的相交信息
    std::optional<SurfaceInteraction> record = scene.intersect(ray);
    // 光线未与场景相交
    if (!record) {
      if (bounce == 0 || specularBounce)
        for (const auto& envlight : scene.infinitAreaLights) {
          Li += beta * envlight->Le(rayIn);
        }
      break;
    } else {
      // 光线与场景相交，相交的信息都储存在record中
      Point3f  p  = record->p;
      Vector3f wo = Normalize(-ray.dir);
      Normal3f n  = record->n;

      // 只有从相机出发的光线击中光源才直接返回光源的emittion
      if (bounce == 0 || specularBounce) {
        // 从相机出发的光线直接击中光源
        if (record->hitLight) {
          Spectrum emittion = record->light->Li(*record, wo);
          Li                += beta * emittion;
          return Li;
        }
      }

      // 采样光源
      if (strategy == LightSampleStrategy::UniformlySampleAllLights) {
        Spectrum L_dir = SampleAllLights(*record, scene, *(threadSampler),
                                         nLightSamples, false);
        Li             += beta * L_dir;
      } else {
        Spectrum L_dir =
            SampleOneLight(*record, scene, *(threadSampler), false);
        Li += beta * L_dir;
      }

      // 采样brdf，计算出射方向,更新beta
      auto [f, wi, pdf, sampledType] =
          record->bsdf->sample_f(wo, threadSampler->get2D());
      if (f == Spectrum(0.0) || pdf == 0.0)
        break;

      // 计算衰减
      double cosTheta = abs(Dot(wi, n));
      beta            *= f * cosTheta / pdf;
      // std::cout <<f<< beta << std::endl;
      specularBounce = (sampledType & BxDFType::Specular) != 0;
      ray            = record->scatterRay(wi);

      // 俄罗斯轮盘赌结束循环
      if (bounce > 3) {
        double q = Max((double).05, 1 - beta.y());
        if (threadSampler->get1D() < q)
          break;
        beta /= 1 - q;
      }
    }
  }
  return Li;
}

void PathTracingIntegrator::preProcess(const Scene& scene) {
  if (strategy == LightSampleStrategy::UniformlySampleAllLights) {
    for (const auto& light : scene.lights)
      nLightSamples.push_back(sampler->roundCount(light->nSamples));

    // request sample arrays
    for (int i = 0; i < maxDepth; i++)
      for (size_t j = 0; j < scene.lights.size(); j++) {
        // two 2D arrays will be used per intersection per light
        sampler->request2DArray(nLightSamples[j]);
        sampler->request2DArray(nLightSamples[j]);
      }
  }
  // request sampler array
}

// Ref<PathTracingIntegrator> PathTracingIntegrator::construct(
//     const PropertyList& param) {
//   int    spp      = param.getInteger("spp", 5);
//   int    maxDepth = param.getInteger("maxDepth", 10);
//   double epsilon  = param.getFloat("epsilon", 1e-6);
//   return std::make_shared<PathTracingIntegrator>(spp, maxDepth, epsilon);
// }

// 实例化注册类静态对象
// PathTracingIntegratorReg PathTracingIntegratorReg::regHelper;

}  // namespace Raven