#include <Raven/sampler/stratified.h>

#include "Raven/core/base.h"
#include "Raven/core/sampler.h"

namespace Raven {
StratifiedSampler::StratifiedSampler(int  xSample,
                                     int  ySample,
                                     bool jitter,
                                     int  nDimensions)
    : PixelSampler(xSample * ySample, nDimensions),
      xPixelSamples(xSample),
      yPixelSamples(ySample),
      jitter(jitter) {}

void StratifiedSampler::startPixel(const Point2i &p) {
  // generate single sample vector for every dimension
  //
  //  generate 1D sample
  for (size_t i = 0; i < samples1D.size(); i++) {
    StratifiedSample1D(&samples1D[i][0], samplesPerPixel, jitter);
    Shuffle(&samples1D[i][0], samplesPerPixel, 1);
  }

  // generate 2D sample
  for (size_t i = 0; i < samples2D.size(); i++) {
    StratifiedSample2D(&samples2D[i][0], xPixelSamples, yPixelSamples, jitter);
    Shuffle(&samples2D[i][0], samplesPerPixel, 1);
  }

  // generate sample arraies
  //
  // generate 1D array sample 
  for (size_t i = 0; i < arraySize1D.size(); i++) {
    for (int64_t j = 0; j < samplesPerPixel; j++) {
      int count = arraySize1D[i];
      StratifiedSample1D(&array1D[i][j * count], count, jitter);
      Shuffle(&array1D[i][j * count], count, 1);
    }
  }

  // generate 2D array sample
  for (size_t i = 0; i < arraySize2D.size(); i++) {
    for (int64_t j = 0; j < samplesPerPixel; j++) {
      int count = arraySize2D[i];
      LatinHyperCube(&array2D[i][j*count].x, count, 2);
    }
  }
}

void StratifiedSample1D(Float *sample, int nSamples, bool jitter) {
  Float invStrata = 1.0 / (Float)nSamples;  // 栅格的宽度
  for (int i = 0; i < nSamples; i++) {
    Float delta = jitter ? GetRand() : 0.5;  // 样本在栅格内的偏移量
    sample[i]   = Min((i + delta) * invStrata, OneMinusEpsilon);
  }
}

void StratifiedSample2D(Point2f *samples,
                        int      xSamples,
                        int      ySamples,
                        bool     jitter) {
  // uniformly stratify samples in x and y dimension respectively
  Float invStrataX = 1.0 / (Float)xSamples;
  Float invStrataY = 1.0 / (Float)ySamples;
  for (int y = 0; y < ySamples; y++) {
    for (int x = 0; x < xSamples; x++) {
      Float deltaX = jitter ? GetRand() : 0.5;
      Float deltaY = jitter ? GetRand() : 0.5;
      samples->x   = Min((x + deltaX) * invStrataX, OneMinusEpsilon);
      samples->y   = Min((y + deltaY) * invStrataY, OneMinusEpsilon);
      ++samples;
    }
  }
}

/// <summary>
/// 随机置换打乱当前维度的样本，might be buggy
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="samples"></param>
/// <param name="count"></param>
/// <param name="nDimension"></param>
template <class T>
void Shuffle(T *samples, int count, int nDimension) {
  for (int i = 0; i < count; i++) {
    // generate random number between 0 and count
    int other = i + GetRandInt(0, count - i);

    // permute samples with the random number generated
    for (int j = 0; j < nDimension; j++)
      std::swap(samples[nDimension * i + j], samples[nDimension * other + j]);
  }
}

//
void LatinHyperCube(Float *sample, int nSamples, int nDimension) {
  // uniformly generqte n jittered samples alone the diagnal of nDimension space
  int invNSample = 1.0 / nSamples;
  for (int i = 0; i < nDimension; i++)
    for (int j = 0; j < nSamples; j++) {
      Float delta        = GetRand();
      Float sampleLocate = (i + delta) * invNSample;

      sample[nDimension * i + j] = Min(sampleLocate, OneMinusEpsilon);
    }

  // permute samples generated
  for (int i = 0; i < nDimension; i++) {
    for (int j = 0; j < nSamples; j++) {
      int other = j + GetRandInt(0, nSamples - j);
      std::swap(sample[nDimension * j + i], sample[nDimension * other + i]);
    }
  }
}
}  // namespace Raven