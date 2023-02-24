#ifndef _RAVEN_CORE_SAMPLER_H_
#define _RAVEN_CORE_SAMPLER_H_

#include <Raven/core/base.h>
#include <Raven/core/math.h>
#include <Raven/core/object.h>

namespace Raven {

static const Float OneMinusEpsilon = 0x1.fffffffffffffp-1;

struct CameraSample;

/// <summary>
/// 采样器接口
/// </summary>
class Sampler : public RavenObject {
 public:
  Sampler(int64_t spp) : samplesPerPixel(spp) {}

  virtual void startPixel(const Point2i& p);

  // 获取样本
  virtual Float   get1D() = 0;
  virtual Point2f get2D() = 0;

  CameraSample getCameraSample(const Point2i& pRaster);

  //allocate memory for sample arrays 
  //allocate size of n 1D/2D samples for each sample in pixel
  void request1DArray(int n);
  void request2DArray(int n);

  virtual int roundCount(int n) const { return n; }

  const Float*   get1DArray(int n);
  const Point2f* get2DArray(int n);

  // 直到最初被请求的n维样本被全部生成完毕前，该函数都return true
  virtual bool startNextSample();

  virtual std::unique_ptr<Sampler> clone(int seed) = 0;

  virtual bool setSampleNumber(int64_t num);

  int64_t getSpp() const { return samplesPerPixel; }

 protected:
  const int64_t samplesPerPixel;
  Point2i       currentPixel;       // 当前采样的像素
  int64_t currentPixelSampleIndex;  // 当前像素内的第i个样本向量
  std::vector<int>                  arraySize1D;
  std::vector<int>                  arraySize2D;
  std::vector<std::vector<Float>>   array1D;
  std::vector<std::vector<Point2f>> array2D;

 private:
  size_t array1DOffset;
  size_t array2DOffset;
};

/// <summary>
/// Pixel Sampler,总是在一个像素内生成所有的sample vector后再移动到下一个像素
/// </summary>
class PixelSampler : public Sampler {
 public:
  PixelSampler(int64_t spp, int nDimensions);

  PixelSampler(const PixelSampler& sampler);

  virtual void startPixel(const Point2i& p) override;

  virtual bool startNextSample() override;

  virtual bool setSampleNumber(int64_t num) override;

  virtual Float   get1D() override;
  virtual Point2f get2D() override;

 protected:
  std::vector<std::vector<Float>>   samples1D;
  std::vector<std::vector<Point2f>> samples2D;
  int                               current1DDimension = 0;
  int                               current2DDimension = 0;
};

class GlobalSampler : public Sampler {
 public:
  GlobalSampler(int64_t nSamples) : Sampler(nSamples) {}

  // 给出位于当前像素的第n个样本的index
  virtual int64_t getSampleIndex(int64_t n) const = 0;

  // 获取目标样本在指定维度的值
  virtual Float sampleDimension(int64_t index, int dimension) const = 0;

  virtual void startPixel(const Point2i& p) override;

  virtual bool startNextSample() override;

  virtual bool setSampleNumber(int64_t num) override;

  virtual Float get1D() override;

  virtual Point2f get2D() override;

 private:
  int dimension;  // 采样器将要生成的样本的下一维度
  int64_t
      currentSampleIndex;  // 采样器在当前像素生成第i个样本在所有样本向量中的index
  static const int arrayStartDim = 5;
  int              arrayEndDim;
};

struct CameraSample {
  CameraSample(const Point2f& film, double t, const Point2f& lens)
      : filmSample(film), time(t), lensSample(lens) {}

  CameraSample(double fu, double fv, double t, double lu, double lv)
      : filmSample(Point2f(fu, fv)), time(t), lensSample(Point2f(lu, lv)) {}

  Point2f filmSample;
  double  time;
  Point2f lensSample;
};

}  // namespace Raven
#endif