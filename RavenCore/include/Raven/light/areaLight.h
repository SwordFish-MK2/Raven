#ifndef _RAVEN_LIGHT_AREA_LIGHT_H_
#define _RAVEN_LIGHT_AREA_LIGHT_H_

#include <Raven/core/base.h>
#include <Raven/core/light.h>
#include <Raven/utils/factory.h>
#include <Raven/utils/propertylist.h>

namespace Raven {
/// <summary>
/// 面积光源虚基类
/// </summary>
class AreaLight : public Light {
 public:
  AreaLight(const Ref<Transform>& LTW,
            const Ref<Transform>& WTL,
            int                   flag,
            int                   nSamples,
            const Ref<Shape>&     shape)
      : Light(LTW, WTL, flag, nSamples),
        shape_ptr(shape),
        area(shape->area()) {}

  // 输入空间中的一个点p，在光源上随机采样，并计算出射的Radiance
  virtual std::optional<LightSample> sampleLi(
      const Interaction& inter,
      const Point2f&     uv) const override = 0;

  // 给定光源上的一个点与出射方向，计算出射的Radiance
  virtual Spectrum Li(const Interaction& inter, const Vector3f& wi) const = 0;

  // 返回光源向空间中辐射的总的能量
  virtual Spectrum power() const override = 0;

  // 给定光源上的一个点与出射方向，计算采样的pdf
  virtual double pdf_Li(const Interaction& inter,
                        const Vector3f&    wi) const override = 0;

  // virtual void setShapePtr(const Ref<Shape>& sptr) { shape_ptr = sptr; }
 protected:
  Ref<Shape>   shape_ptr;
  const double area;
};

/// <summary>
/// DiffuseAreaLight实现类，该光源上的任意一点向空间中的任意方向辐射同等大小的Radiance
/// </summary>
class DiffuseAreaLight final : public AreaLight {
 public:
  DiffuseAreaLight(const Ref<Transform>& LTW,
                   const Ref<Transform>& WTL,
                   int                   nSamples,
                   const Ref<Shape>&     shape,
                   const Spectrum&       I)
      : AreaLight(LTW, WTL, (int)LightFlag::AreaLight, nSamples, shape),
        emittedRadiance(I) {}

  virtual Spectrum Li(const Interaction& p, const Vector3f& wi) const override;

  virtual std::optional<LightSample> sampleLi(const Interaction& inter,
                                              const Point2f& uv) const override;

  virtual Spectrum power() const override;

  virtual double pdf_Li(const Interaction& inter,
                        const Vector3f&    wi) const override;

  // static Ref<Light> construct(const PropertyList& param) {
  //   const int        n     = param.getInteger("nSamples", 1);
  //   const Spectrum   I     = param.getSpectra("intansity", Spectrum(1.0));
  //   const ObjectRef  s     = param.getObjectRef(0);
  //   const Ref<Shape> shape = std::dynamic_pointer_cast<Shape>(s.getRef());
  //   return std::make_shared<DiffuseAreaLight>(nullptr, nullptr, n, shape, I);
  // }

 private:
  const Spectrum emittedRadiance;
};

// _RAVEN_CLASS_REG_(area, DiffuseAreaLight, DiffuseAreaLight::construct)

}  // namespace Raven

#endif