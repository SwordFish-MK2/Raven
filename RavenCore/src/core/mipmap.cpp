#include <Raven/core/mipmap.h>
#include <omp.h>

#include <sstream>

namespace Raven {
template <class T>
Mipmap<T>::Mipmap(const Image<T>& imageData, bool trilinear, ImageWrap wrap)
    : doTrilinear(trilinear),
      resolution(imageData.uSize(), imageData.vSize()),
      wrapMode(wrap) {
  Image<T> resampledImage(resolution.x, resolution.y);
  Point2i  resampledRes = resolution;

  // 如果图片的分辨率不是2的指数，将其放大到2的指数倍
  if (!IsPowerOf2(resampledRes.x) || !IsPowerOf2(resampledRes.y)) {
    resampledRes =
        Point2i(RoundUpPower2(resampledRes.x), RoundUpPower2(resampledRes.y));
    resampledImage.resize(resampledRes.x, resampledRes.y);

    // 在x方向拉伸图像
    Image<T>                    xResampled(resampledRes.x, resolution.y);
    std::vector<ResampleWeight> xWeights =
        resampleWeights(resolution.x, resampledRes.x);
#pragma omp parallel for
    for (int t = 0; t < resolution.y; t++) {
      for (int s = 0; s < resampledRes.x; s++) {
        for (int i = 0; i < 4; i++) {
          double w = xWeights[s].weight[i];

          int32_t xOffset = xWeights[s].firstTexel + i;
          if (wrapMode == ImRepeat)
            xOffset = (xOffset + resolution.x) % resolution.x;
          if (wrapMode == ImClamp)
            xOffset = Clamp(xOffset, 0, (int64_t)resolution.x - 1);

          if (xOffset >= 0 && xOffset < resolution.x)
            xResampled(s, t) += imageData(xOffset, t) * w;
        }
      }
    }

    // 在y方向拉伸图像
    std::vector<ResampleWeight> yWeights =
        resampleWeights(resolution.y, resampledRes.y);
#pragma omp parallel for
    for (int s = 0; s < resampledRes.x; s++) {
      for (int t = 0; t < resampledRes.y; t++) {
        for (int i = 0; i < 4; i++) {
          double w = yWeights[t].weight[i];

          int32_t yOffset = yWeights[t].firstTexel + i;
          if (wrapMode == ImageWrap::ImRepeat)
            yOffset = (yOffset + resolution.y) % resolution.y;
          if (wrapMode == ImageWrap::ImClamp)
            yOffset = Clamp(yOffset, 0, (int64_t)resolution.y - 1);

          if (yOffset >= 0 && yOffset < resolution.y)
            resampledImage(s, t) += xResampled(s, yOffset) * w;
        }
      }
    }
  } else {
    resampledImage = imageData;
  }

  // 初始化图像金字塔
  maxLevel = 1 + static_cast<int>(Log2(Max(resampledRes.x, resampledRes.y)));
  pyramid.resize(maxLevel);

  // 金字塔底层为拉伸后的原图像
  pyramid[0].reset(
      new Image<T>(resampledRes.x, resampledRes.y, &resampledImage[0]));

  int sRes = resampledRes.x;
  int tRes = resampledRes.y;

  // 生成第i层图像
  for (int i = 1; i < maxLevel; i++) {
    // 计算第i层图像的分辨率
    sRes = Max(1, sRes / 2);
    tRes = Max(1, tRes / 2);
    pyramid[i].reset(new Image<T>(sRes, tRes));

    // 用box filter处理上一层的纹素得到该层纹素的值
#pragma omp parallel for
    for (int t = 0; t < tRes; t++) {
      for (int s = 0; s < sRes; s++) {
        (*pyramid[i])(s, t) = 0.25 * (texel(i - 1, s * 2, t * 2) +
                                      texel(i - 1, s * 2 + 1, t * 2) +
                                      texel(i - 1, s * 2, t * 2 + 1) +
                                      texel(i - 1, s * 2 + 1, t * 2 + 1));
      }
    }
  }

  // log();
}

// compute weights of original texels to resampled texels
template <class T>
std::vector<ResampleWeight> Mipmap<T>::resampleWeights(int32_t oldRes,
                                                       int32_t newRes) {
  std::vector<ResampleWeight> weights     = std::vector<ResampleWeight>(newRes);
  double                      filterWidth = 2.;
  for (int i = 0; i < newRes; i++) {
    // compute continuous texture coordinates of resampled texel relative to
    // original sample coordinates
    double coord = (i + 0.5) * oldRes / newRes;

    // compute index of first texel contributes to new texel
    weights[i].firstTexel = std::floor(coord - filterWidth + 0.5);

    // compute weights of 4 adjcent texels
    for (int j = 0; j < 4; j++) {
      double pos = weights[i].firstTexel + j +
                   0.5;  // convert discret coordinate to continuous coords
      weights[i].weight[j] =
          lanczosSinc(abs(coord - pos));  // compute weight using sinc function
    }

    // normalize weights so that they sum to 1
    double invSum = 1 / (weights[i].weight[0] + weights[i].weight[1] +
                         weights[i].weight[2] + weights[i].weight[3]);
    for (int j = 0; j < 4; j++) weights[i].weight[j] *= invSum;
  }
  return weights;
}

// 输入纹理坐标st与filter的宽度，从Mipmap中取一个值
template <class T>
T Mipmap<T>::lookup(const Point2f& st, double width) const {
  // 根据filter宽度计算一个浮点数表示的层数
  double level = pyramid.size() - 1 + Log2(Max(width, 1e-8));

  if (level < 0)
    return triangle(0, st);  // 返回在原图像st纹理坐标下的值

  else if (level > (int64_t)maxLevel - 1)
    return texel(maxLevel - 1, 0, 0);  // 最大层的Mipmap只有一个固定值

  else {
    // triangle filter both image level beside and linear interpolate two
    // filtered value 在邻近的两层中用三角滤波求得st纹理坐标下的值并插值
    int    levelFlr   = (int)std::floor(level);
    double delta      = level - levelFlr;
    T      vLastLevel = triangle(levelFlr, st);
    T      vNextLevel = triangle(levelFlr + 1, st);
    return Lerp(delta, vLastLevel, vNextLevel);
  }
}

// 输入纹理坐标st与偏导数，从Mipmap中取一个值
template <class T>
T Mipmap<T>::lookup(const Point2f&  st,
                    const Vector2f& dstdx,
                    const Vector2f& dstdy) const {
  // 纹理滤波的宽度为四个偏导数的最大值
  double filterWidth =
      Max(Max(abs(dstdx.x), abs(dstdx.y)), Max(abs(dstdy.x), abs(dstdy.y)));
  return lookup(st, filterWidth);
}

template <class T>
void Mipmap<T>::log() {
  const char* command = "mkdir mipmap";
  system(command);

  std::ostringstream ss("mipmap/level", std::ios_base::ate);
  for (size_t i = 0; i < pyramid.size(); i++) {
    ss << i << ".jpg";
    std::string filename = ss.str();
    ss.str("mipmap/level");
    Image<T>* l = getLevel(i);
    WriteImage(*l, filename);
  }
}

// 双线性插值
template <class T>
T Mipmap<T>::triangle(int level, const Point2f& st) const {
  level = Clamp(level, 0, (int64_t)maxLevel - 1);
  // compute continous coordinate - 0.5 of sample point in order to compute
  // distance
  float s = st[0] * pyramid[level]->uSize() - 0.5f;
  float t = st[1] * pyramid[level]->vSize() - 0.5f;

  int is = (int)std::floor(s);
  int it = (int)std::floor(t);
  // perform bilinear interpolate between four adjacent texels
  float ds       = s - is;
  float dt       = t - it;
  T     sLerped0 = Lerp(ds, texel(level, is, it), texel(level, is + 1, it));
  T sLerped1 = Lerp(ds, texel(level, is, it + 1), texel(level, is + 1, it + 1));
  return Lerp(dt, sLerped0, sLerped1);
}

template <class T>
const T& Mipmap<T>::texel(int level, int s, int t) const {
  // get image data of input level
  const Image<T>& l = *pyramid[level];
  // bound st coordinates according to image wrap mode
  switch (wrapMode) {
    case ImRepeat:
      s = (s + l.uSize()) % l.uSize();
      t = (t + l.vSize()) % l.vSize();
      break;
    case ImClamp:
      s = Clamp(s, 0, l.uSize() - 1);
      t = Clamp(t, 0, l.vSize() - 1);
      break;
    case ImBlack:
      static const T black(
          0.0);  // to avoid returning a reference of a local temporary object
      if (s < 0 || s > l.uSize() - 1)
        return black;
      if (t < 0 || s > l.vSize() - 1)
        return black;
      break;
  }
  return l(s, t);
}

template <class T>
double Mipmap<T>::lanczosSinc(double v, double tau) const {
  v = v / tau;
  return sin(M_PI * v) / (M_PI * v);
}

template class Mipmap<RGBSpectrum>;
template class Mipmap<double>;
}  // namespace Raven