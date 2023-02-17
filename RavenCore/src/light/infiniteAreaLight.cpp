#include <Raven/core/scene.h>
#include <Raven/light/infiniteAreaLight.h>

#include <memory>

namespace Raven {
InfiniteAreaLight::InfiniteAreaLight(const Ref<Transform>& lightToWorld,
                                     const Ref<Transform>& worldToLight,
                                     const Spectrum&       L,
                                     int                   nSamples,
                                     const std::string&    mapPath)
    : Light(lightToWorld,
            worldToLight,
            (int)LightFlag::InfiniteLight,
            nSamples) {
  Point2i resolution;

  // 加载imageMap
  if (mapPath != "") {
    Image<Spectrum> texels = ReadImage(mapPath);
    // 如果成功读取imageMap
    resolution.x = texels.uSize();
    resolution.y = texels.vSize();
    for (int i = 0; i < texels.uSize() * texels.vSize(); i++) texels[i] *= L;
    lightMap.reset(new Mipmap(texels));
  }

  // 生成sampling weight
  int    width = resolution[0], height = resolution[1];  // weight大小
  double filter = 1.0 / Max(width, height);              // 滤波宽度
  std::unique_ptr<double[]> img(new double[width * height]);
  for (int v = 0; v < height; v++) {
    double vp       = (double)v / (double)height;
    double sinTheta = std::sin(M_PI * double(v + 0.5) /
                               double(height));  // 生成sintheta用于消除扭曲
    for (int u = 0; u < width; u++) {
      double up          = double(u) / double(width);
      img[u + v * width] = lightMap->lookup(Point2f(up, vp), filter)
                               .y();  // 使用采样到的Spectrum的亮度来生成pdf
      img[u + v * width] *= sinTheta;
    }
  }
  distribution.reset(new Distribution2D(img.get(), width, height));
}

void InfiniteAreaLight::preprocess(const Scene& scene) {
  scene.worldBound().boundingSphere(worldCenter, worldRadius);
}

Spectrum InfiniteAreaLight::power() const {
  return M_PI * worldRadius * worldRadius *
         Spectrum(lightMap->lookup(Point2f(0.5, 0.5), 0.5),
                  RGBType::RGBIllumination);
}

Spectrum InfiniteAreaLight::sampleLi(const Interaction& inter,
                                     const Point2f&     rand,
                                     LightSample*       lightSample) const {
  double  pdf = 0;
  Point2f uv =
      distribution->sampleContinuous(rand, &pdf);  // 根据光源强度分布生成样本
  if (pdf == 0)
    return Spectrum(0);

  // 计算采样光线的方向
  double   theta = M_PI * uv[1], phi = 2 * M_PI * uv[0];
  double   cosTheta = std::cos(theta), sinTheta = std::sin(theta);
  double   cosPhi = std::cos(phi), sinPhi = std::sin(phi);
  Vector3f wi = Normalize((*lightToWorld)(
      Vector3f(sinTheta * cosPhi, cosTheta, -sinTheta * sinPhi)));

  // 将密度函数从uv坐标系转化为方向坐标
  pdf = pdf / (2 * M_PI * M_PI * sinTheta);
  if (sinTheta == 0)
    pdf = 0;

  // 将入射光原点设置在两倍场景半径之外
  Point3f lightp   = inter.p + 2 * worldRadius * wi;
  lightSample->p   = lightp;
  lightSample->pdf = pdf;
  lightSample->wi  = wi;
  return Spectrum(lightMap->lookup(uv));
}

double InfiniteAreaLight::pdf_Li(const Interaction& sinter,
                                 const Vector3f&    w) const {
  Vector3f wLight   = (*worldToLight)(w);
  double   theta    = SphericalTheta(wLight);
  double   phi      = SphericalPhi(wLight);
  double   sinTheta = std::sin(theta);
  if (sinTheta == 0)
    return 0;
  return distribution->pdf(Point2f(phi * 0.5 / M_PI, theta / M_PI) /
                           (2 * M_PI * M_PI * sinTheta));
}

Spectrum InfiniteAreaLight::Le(const RayDifferential& ray) const {
  const Vector3f w      = Normalize((*worldToLight)(ray.dir));
  double         inv2Pi = 0.5 / M_PI;
  Point2f        st(SphericalTheta(w) * inv2Pi, SphericalPhi(w) * inv2Pi);
  return Spectrum(lightMap->lookup(st));
}
}  // namespace Raven