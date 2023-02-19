#ifndef _RAVEN_LIGHT_INFINITE_AREA_LIGHT_H_
#define _RAVEN_LIGHT_INFINITE_AREA_LIGHT_H_

#include <Raven/core/base.h>
#include <Raven/core/distribution.h>
#include <Raven/core/light.h>
#include <Raven/core/mipmap.h>

namespace Raven {
class InfiniteAreaLight final : public Light {
 public:
  InfiniteAreaLight(const Ref<Transform>& LightToWorld,
                    const Ref<Transform>& worldToLight,
                    const Spectrum&       L,
                    int                   nSamples,
                    const std::string&    textureMap);
  // 输入空间中的一个点p，在光源上随机采样，并计算出射的Radiance
  std::optional<LightSample> sampleLi(const Interaction& inter,
                                      const Point2f&     uv) const override;

  // 返回光源向空间中辐射的总的能量
  virtual Spectrum power() const override;

  // 给定光源上的一个点与出射方向，计算采样的pdf
  virtual double pdf_Li(const Interaction& inter,
                        const Vector3f&    wi) const override;

  Spectrum Le(const RayDifferential&) const override;

  void preprocess(const Scene& scene) override;

 private:
  std::unique_ptr<Mipmap<RGBSpectrum>> lightMap;
  std::unique_ptr<Distribution2D>      distribution;
  const Transform lightWorld;  // 用于调整image map的方向，决定哪个方向为上
  Point3f worldCenter;
  double  worldRadius;
};

}  // namespace Raven
#endif