#include<Raven/core/sampler.h>

namespace Raven {
	void Sampler::startPixel(const Point2i& p) {
		currentPixel = p;
		currentPixelSampleIndex = 0;
		array1DOffset = 0;
		array2DOffset = 0;
	}

	bool Sampler::startNextSample() {
		array1DOffset = 0;
		array2DOffset = 0;
		return ++currentPixelSampleIndex < samplesPerPixel;
	}

	bool Sampler::setSampleNumber(int num) {
		array1DOffset = 0;
		array2DOffset = 0;
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

	PixelSampler::PixelSampler(
		int64_t spp,
		int nDimensions
	) :
		Sampler(spp),
		nDimensions(nDimensions)
	{
		for (int i = 0; i < nDimensions; i++) {
			samples1D.push_back(std::vector<Float>(samplesPerPixel));
			samples2D.push_back(std::vector<Point2f>(samplesPerPixel));//?
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

	bool PixelSampler::setSampleNumber(int num) {
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
}