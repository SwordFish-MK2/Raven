#ifndef _RAVEN_CORE_SAMPLER_H_
#define _RAVEN_CORE_SAMPLER_H_

#include<Raven/core/base.h>
#include<Raven/core/object.h>
#include<Raven/core/math.h>

namespace Raven {

	/// <summary>
	/// �������ӿ�
	/// </summary>
	class Sampler :public RavenObject {
	public:
		Sampler(int64_t spp) :samplePerPixel(spp) {}

		virtual void startPixel(const Point2f& p);

		//��ȡ����
		virtual Float get1D() = 0;
		virtual Float get2D() = 0;

		//CameraSample getCameraSample(const Point2f& pRaster);

		void request1DArray(int n);
		void request2DArray(int n);

		virtual roundCount(int n)const { return n; }

		const Float* get1DArray(int n);
		const Point2f* get2DArray(int n);

		//ֱ������������nά������ȫ���������ǰ���ú�����return true
		virtual bool startNextSample();

		virtual std::unique_ptr<Sampler> clone(int seed) = 0;

		virtual bool setSampleNumber(int num);

	protected:
		const int64_t samplePerPixel;
		Point2i currentPixel;					//��ǰ����������
		int64_t currentPixelSampleIndex;		//��ǰ�����ڵĵ�i����������
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