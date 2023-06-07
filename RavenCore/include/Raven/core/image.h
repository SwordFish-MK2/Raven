#ifndef _RAVEN_CORE_IMAGE_H_
#define _RAVEN_CORE_IMAGE_H_

#include <Raven/core/base.h>
#include <Raven/core/math.h>
#include <Raven/core/spectrum.h>

#include <cstring>

namespace Raven {

enum class ImageFormat {
  Float_3Channel = 0,
  Float_1Channel,
  RGB_3Channel,
  RGB_1Channel
};

template <class T>
class Image {
 public:
  Image(int width, int height, const T* d = nullptr)
      : xRes(width), yRes(height) {
    size_t dataSize = xRes * yRes;
    data.resize(dataSize);
    if (d != nullptr)
      std::memcpy(&data[0], d, dataSize * sizeof(T));
  }

  Image(const Image& image)
      : xRes(image.xRes), yRes(image.yRes), data(image.data) {}

  int uSize() const { return xRes; }
  int vSize() const { return yRes; }

  void resize(int width, int height) {
    xRes        = width;
    yRes        = height;
    int newSize = width * height;
    data.resize(newSize);
  }

  void setPixel(const T& value, int x, int y) {
    int index   = y * xRes + x;
    data[index] = value;
  }
  T getPixel(const T& value, int x, int y) const {
    int index = y * xRes + x;
    return data[index];
  }

  T& operator()(int x, int y) {
    int index = y * xRes + x;
    return data[index];
  }
  const T& operator()(int x, int y) const {
    int index = y * xRes + x;
    return data[index];
  }

  T&      operator[](int i) { return data[i]; }
  const T operator[](int i) const { return data[i]; }

  static Image<T> Read(const std::string& path, ImageFormat format);

 private:
  int            xRes, yRes;
  std::vector<T> data;
};

Spectrum toSpectrum(const unsigned char* data);

Image<RGBSpectrum> ReadImage(const std::string& path);

void WriteImage(const Image<RGBSpectrum>& image, const std::string& filename);

void WriteImage(const Image<double>& image, const std::string& filename);

using Image3 = Image<Spectrum>;
using Image1 = Image<Float>;
}  // namespace Raven

#endif