#ifndef _RAVEN_SAMPLER_STRATIFIED_H_
#define _RAVEN_SAMPLER_STRATIFIED_H_

#include <Raven/core/base.h>
#include <Raven/core/sampler.h>

#include <cstdint>

namespace Raven {
/// <summary>
/// 分层采样
/// </summary>
class StratifiedSampler final : public PixelSampler {
public:
  StratifiedSampler(int xSamples, int ySamples, bool jitter, int nDimensions);

  void startPixel(const Point2i &p) override;

//  std::unique_ptr<Sampler> clone(int seed) override;

private:
  const int xPixelSamples;
  const int yPixelSamples;
  bool      jitter;
};

void LatinHyperCube(Float *sample, int nSamples, int nDimension);

void StratifiedSample1D(Float *sample, int nSamples, bool jitter);

void StratifiedSample2D(Point2f *sample,
                        int      xSamples,
                        int      ySamples,
                        bool     jitter);

template <class T>
void Shuffle(T *samples, int count, int nDimension);
}  // namespace Raven

#endif