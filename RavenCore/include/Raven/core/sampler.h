#ifndef _RAVEN_CORE_SAMPLER_H_
#define _RAVEN_CORE_SAMPLER_H_

#include<Raven/core/base.h>
#include<Raven/core/object.h>
#include<Raven/core/math.h>

namespace Raven {

	/// <summary>
	/// 采样器接口
	/// </summary>
	class Sampler :public RavenObject {
	public:
		Sampler(int64_t spp) :samplePerPixel(spp) {}

		virtual void startPixel(const Point2f& p);

		//获取样本
		virtual Float get1D() = 0;
		virtual Float get2D() = 0;

		//CameraSample getCameraSample(const Point2f& pRaster);

		void request1DArray(int n);
		void request2DArray(int n);

		virtual roundCount(int n)const { return n; }

		const Float* get1DArray(int n);
		const Point2f* get2DArray(int n);

		//直到最初被请求的n维样本被全部生成完毕前，该函数都return true
		virtual bool startNextSample();

		virtual std::unique_ptr<Sampler> clone(int seed) = 0;

		virtual bool setSampleNumber(int num);

	protected:
		const int64_t samplePerPixel;
		Point2i currentPixel;					//当前采样的像素
		int64_t currentPixelSampleIndex;		//当前像素内的第i个样本向量
		std::vector<int>arraySize1D;
		std::vector<int>arraySize2D;
		std::vector<std::vector<Float>> array1D;
		std::vector<std::vector<Point2f>> array2D;

	private:
		size_t array1DOffset;
		size_t array2DOffset;
	};

	class PixelSampler :public Sampler {
	public:
		PixelSampler(int64_t spp, int nDimensions);

	protected:
		std::vector<std::vector<Float>> samples1D;
		std::vector<std::vector<Point2f>> samples2D;
		int current1DDimension = 0;
		int current2DDimension = 0;
	private:
		const int nDimensions;
	};
}
#endif