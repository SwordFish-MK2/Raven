#include <Raven/core/sampler.h>

namespace Raven {
void Sampler::startPixel(const Point2i& p) {
  currentPixel            = p;
  currentPixelSampleIndex = 0;
  array1DOffset           = 0;
  array2DOffset           = 0;
}

bool Sampler::startNextSample() {
  array1DOffset = 0;
  array2DOffset = 0;
  return ++currentPixelSampleIndex < samplesPerPixel;
}

CameraSample Sampler::getCameraSample(const Point2i& pRaster) {
  Point2f pFilm = Point2f((Float)pRaster.x, (Float)pRaster.y) + get2D();
  Float   time  = get1D();
  Point2f pLens = get2D();
  return CameraSample(pFilm, time, pLens);
}

bool Sampler::setSampleNumber(int64_t num) {
  array1DOffset           = 0;
  array2DOffset           = 0;
  currentPixelSampleIndex = num;
  return currentPixelSampleIndex < samplesPerPixel;
}

void Sampler::request1DArray(int n) {
  arraySize1D.push_back(n);
  array1D.push_back(std::vector<Float>(n * samplesPerPixel));
}

void Sampler::request2DArray(int n) {
  arraySize2D.push_back(n);
  array2D.push_back(std::vector<Point2f>(n * samplesPerPixel));
}

const Float* Sampler::get1DArray(int n) {
  if (array1DOffset == array1D.size())
    return nullptr;
  return &array1D[array1DOffset++][n * currentPixelSampleIndex];
}

const Point2f* Sampler::get2DArray(int n) {
  if (array2DOffset == array2D.size())
    return nullptr;
  return &array2D[array2DOffset++][n * currentPixelSampleIndex];
}

PixelSampler::PixelSampler(int64_t spp, int nDimensions)
    : Sampler(spp), nDimensions(nDimensions) {
  for (int i = 0; i < nDimensions; i++) {
    samples1D.push_back(std::vector<Float>(samplesPerPixel));
    samples2D.push_back(std::vector<Point2f>(samplesPerPixel));  //?
  }
}

void PixelSampler::startPixel(const Point2i& p) {
  current1DDimension = 0;
  current2DDimension = 0;
  return Sampler::startPixel(p);
}

bool PixelSampler::startNextSample() {
  current1DDimension = 0;
  current2DDimension = 0;
  return Sampler::startNextSample();
}

bool PixelSampler::setSampleNumber(int64_t num) {
  current1DDimension = 0;
  current2DDimension = 0;
  return Sampler::setSampleNumber(num);
}

Float PixelSampler::get1D() {
  if (current1DDimension < samples1D.size())
    return samples1D[current1DDimension++][currentPixelSampleIndex];
  else
    return GetRand();
}

Point2f PixelSampler::get2D() {
  if (current2DDimension < samples2D.size())
    return samples2D[current2DDimension++][currentPixelSampleIndex];
  else
    return Point2f(GetRand(), GetRand());
}

void GlobalSampler::startPixel(const Point2i& p) {
  Sampler::startPixel(p);
  dimension = 0;

  // 设置当前样本的index为落在此像素中的第一个sample的index
  currentSampleIndex = getSampleIndex(0);

  arrayEndDim = arrayStartDim + array1D.size() + 2 * array2D.size();

  for (size_t i = 0; i < arraySize1D.size(); i++) {
    int currentDimensionSampleNumber = samplesPerPixel * arraySize1D[i];
    for (size_t j = 0; j < currentDimensionSampleNumber; j++) {
      int64_t index = getSampleIndex(j);
      array1D[i][j] = sampleDimension(index, arrayStartDim + i);
    }
  }

  // 2维样本数组的起始维度在一位样本数组之后
  int dim = arrayStartDim + arraySize1D.size();
  for (size_t i = 0; i < arraySize2D.size(); i++) {
    int currentDimensionSampleNumber = samplesPerPixel * arraySize2D[i];
    for (size_t j = 0; j < currentDimensionSampleNumber; j++) {
      int64_t index   = getSampleIndex(j);
      array2D[i][j].x = sampleDimension(index, dim);
      array2D[i][j].y = sampleDimension(index, dim + 1);
    }
    dim += 2;
  }
}

/// <summary>
/// 开始采样一个新的样本时，设置维度为0并且重新获取该样本的索引
/// currentSampleIndex：当前采样的样本在所有样本中的索引
/// currentPixelSampleIndex：当前正在采样当前像素的第i个样本
/// </summary>
/// <returns></returns>
bool GlobalSampler::startNextSample() {
  dimension          = 0;
  currentSampleIndex = getSampleIndex(currentPixelSampleIndex + 1);
  return Sampler::startNextSample();
}

bool GlobalSampler::setSampleNumber(int64_t num) {
  dimension          = 0;
  currentSampleIndex = getSampleIndex(num);
  return Sampler::setSampleNumber(num);
}

/// <summary>
/// 获取1D或者2D的非数组样本时，直接跳过样本数组所在维度，从更高维度采样
/// </summary>
/// <returns></returns>
Float GlobalSampler::get1D() {
  if (dimension >= arrayStartDim && dimension < arrayEndDim)
    dimension = arrayEndDim;
  return sampleDimension(currentSampleIndex, dimension++);
}

Point2f GlobalSampler::get2D() {
  if (dimension >= arrayStartDim && dimension < arrayEndDim)
    dimension = arrayEndDim;
  Point2f sample(sampleDimension(currentSampleIndex, dimension),
                 sampleDimension(currentSampleIndex, dimension + 1));
  dimension += 2;
  return sample;
}
}  // namespace Raven