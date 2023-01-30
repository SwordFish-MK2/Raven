#include<Raven/sampler/stratified.h>

namespace Raven {
	StratifiedSampler::StratifiedSampler(
		int xSample,
		int ySample,
		bool jitter,
		int nDimensions
	) :
		PixelSampler(xSample* ySample, nDimensions),
		xPixelSamples(xSample),
		yPixelSamples(ySample),
		jitter(jitter) {  }

	void StratifiedSampler::startPixel(const Point2i& p)
	{

		//遍历所有1D分量，为每个1D向量分层采样n个样本
		for (size_t i = 0; i < samples1D.size(); i++) {
			StratifiedSample1D(&samples1D[i][0], samplesPerPixel, jitter);
			Shuffle(&samples1D[i][0], samplesPerPixel, 1);
		}

		//遍历所有2D分量，为每个2D分量分层采样n个样本
		for (size_t i = 0; i < samples2D.size(); i++) {
			StratifiedSample2D(&samples2D[i][0], xPixelSamples, yPixelSamples, jitter);
			Shuffle(&samples2D[i][0], samplesPerPixel, 1);
		}
	}

	void StratifiedSample1D(
		Float* sample,
		int nSamples,
		bool jitter)
	{
		Float invStrata = 1.0 / (Float)nSamples;//栅格的宽度
		for (int i = 0; i < nSamples; i++) {
			Float delta = jitter ? GetRand() : 0.5;//样本在栅格内的偏移量
			sample[i] = Min((i + delta) * invStrata, OneMinusEpsilon);
		}
	}

	void StratifiedSample2D(
		Point2f* samples,
		int xSamples,
		int ySamples,
		bool jitter)
	{
		Float invStrataX = 1.0 / (Float)xSamples;
		Float invStrataY = 1.0 / (Float)ySamples;
		for (int y = 0; y < ySamples; y++) {
			for (int x = 0; x < xSamples; x++) {
				Float deltaX = jitter ? GetRand() : 0.5;
				Float deltaY = jitter ? GetRand() : 0.5;
				samples->x = Min((x + deltaX) * invStrataX, OneMinusEpsilon);
				samples->y = Min((y + deltaY) * invStrataY, OneMinusEpsilon);
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
	template<class T>
	void Shuffle(T* samples, int count, int nDimension) {
		for (int i = 0; i < count; i++) {
			static std::uniform_int_distribution<int> distribution(0, count);
			static std::mt19937 g;
			int other = distribution(g);
			for (int j = 0; j < nDimension; j++) {
				std::swap(samples[nDimension * i + j],
					samples[nDimension * other + j]);
			}
		}
	}
}