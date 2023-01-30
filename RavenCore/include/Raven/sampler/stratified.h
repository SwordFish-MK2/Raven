#ifndef _RAVEN_SAMPELR_STRATIFIED_H_
#define _RAVEN_SAMPLER_STRATIFIED_H_

#include<Raven/core/base.h>
#include<Raven/core/sampler.h>

namespace Raven {
	/// <summary>
	/// ·Ö²ã²ÉÑù
	/// </summary>
	class StratifiedSampler final :public PixelSampler {
	public:
		StratifiedSampler(int xSamples, int ySamples, bool jitter, int nDimensions);

		void startPixel(const Point2i& p)override;

	private:
		const int xPixelSamples;
		const int yPixelSamples;
		bool jitter;

	};

	void StratifiedSample1D(Float* sample, int nSamples, bool jitter);

	void StratifiedSample2D(Point2f* sample, int xSamples, int ySamples, bool jitter);

	template<class T>
	void Shuffle(T* samples, int count, int nDimension);
}

#endif