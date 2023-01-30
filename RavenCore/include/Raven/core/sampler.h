#ifndef _RAVEN_CORE_SAMPLER_H_
#define _RAVEN_CORE_SAMPLER_H_

#include<Raven/core/base.h>
#include<Raven/core/object.h>
#include<Raven/core/math.h>

namespace Raven {

	static const Float OneMinusEpsilon = 0x1.fffffffffffffp-1;

	/// <summary>
	/// �������ӿ�
	/// </summary>
	class Sampler :public RavenObject {
	public:
		Sampler(int64_t spp) :samplesPerPixel(spp) {}

		virtual void startPixel(const Point2i& p);

		//��ȡ����
		virtual Float get1D() = 0;
		virtual Point2f get2D() = 0;

		//CameraSample getCameraSample(const Point2f& pRaster);

		void request1DArray(int n);
		void request2DArray(int n);

		virtual int roundCount(int n)const { return n; }

		const Float* get1DArray(int n);
		const Point2f* get2DArray(int n);

		//ֱ������������nά������ȫ���������ǰ���ú�����return true
		virtual bool startNextSample();

		virtual std::unique_ptr<Sampler> clone(int seed) = 0;

		virtual bool setSampleNumber(int64_t num);

	protected:
		const int64_t samplesPerPixel;
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

	/// <summary>
	/// Pixel Sampler,������һ���������������е�sample vector�����ƶ�����һ������
	/// </summary>
	class PixelSampler :public Sampler {
	public:
		PixelSampler(int64_t spp, int nDimensions);

		virtual void startPixel(const Point2i& p)override;

		virtual bool startNextSample()override;

		virtual bool setSampleNumber(int64_t num) override;

		virtual Float get1D() override;
		virtual Point2f get2D() override;

	protected:
		std::vector<std::vector<Float>> samples1D;
		std::vector<std::vector<Point2f>> samples2D;
		int current1DDimension = 0;
		int current2DDimension = 0;
	private:
		const int nDimensions;
	};

	class GlobalSampler :public Sampler {
	public:
		GlobalSampler(int64_t nSamples) :Sampler(nSamples) {}

		//����λ�ڵ�ǰ���صĵ�n��������index
		virtual int64_t getSampleIndex(int64_t n)const = 0;

		//��ȡĿ��������ָ��ά�ȵ�ֵ
		virtual Float sampleDimension(int64_t index, int dimension)const = 0;

		virtual void startPixel(const Point2i& p)override;

		virtual bool startNextSample()override;

		virtual bool setSampleNumber(int64_t num)override;

		virtual Float get1D()override;

		virtual Point2f get2D()override;

	private:
		int dimension;					//��������Ҫ���ɵ���������һά��
		int64_t currentSampleIndex;		//�������ڵ�ǰ�������ɵ�i���������������������е�index
		static const int arrayStartDim = 5;
		int arrayEndDim;
	};
}
#endif