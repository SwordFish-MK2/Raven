#include"mipmap.h"

namespace Raven {
	template<class T>
	Mipmap<T>::Mipmap(const Point2f& resolution, const T* imageData, bool doTrilinear, ImageWrap wrap)
		:wrapMode(wrap), doTrilinear(doTrilinear) {
		std::unique_ptr<T> resampledImage = nullptr;
		//resize the image if it's resolution isn't power of 2 
		pointf2 resampledRes = resolution;
		if (!(IsPowerOf2(resolution.x) || !IsPowerOf2(resolution.y))) {
			resampledRes = pointf2(RoundUpPower2(resolution.x), RoundUpPower2(resolution.y));
			resampledImage.reset(new T[resampledRes.x * resampledRes.y](0.f));//allocate memory

			//scale image in s direction, should be excuted synchronizedly
			std::vector<ReSampleWeight> sWeight = resampleWeights(resolution.x, resampledRes.x);
			for (int t = 0; t < resolution.y; t++) {
				//scale one row
				for (int s = 0; s < resampledRes.x; s++) {
					for (int k = 0; k < 4; k++) {
						//compute origin coordinate
						int32_t origin = sWeight[s].firstTexel + k;
						if (wrapMode == ImRepeat)
							origin = (origin + resolution.x) % resolution.x;
						if (wrapMode == ImClamp)
							origin = Clamp(origin, 0, resolution.x - 1);
						//if wrap mode == ImBlack,coordinates outside legal region return 0, do not need to handle specifically
						resampledImage[t * resampledRes.x + s] = 0.f;
						if (origin >= 0 && origin < resampledRes.x) {
							resampledImage[t * resampledRes.x + s] +=
								imageData[t * resolution.x + origin] * sWeight[s].weight[k];
						}
					}
				}
			}

			//scale image in t direction, should be excuted synchronizedly
			std::unique_ptr<ResampleWeight[]> tWeight = resampleWeights(resolution.y, resampledRes.y);
			for (int s = 0; s < resampledRes.x; s++) {
				//scale one column
				for (int t = 0; t < resampledRes.y; t++) {
					for (int k = 0; k < 4; k++) {
						int32_t oriT = tWeight[t].firstTexel + k;
						if (wrapMode == ImRepeat)
							oriT = (oriT + resolution.x) % resolution.x;
						if (wrapMode == ImClamp)
							oriT = Clamp(oriT, 0, resolution.y - 1);
						if (oriT >= 0 && oriT < resolution.y) {
							resampledImage[s + resampledRes.x * t] +=
								resampledImage[s + resampledRes.x * oriT] * tWeight[t].weight[k];
						}
					}
				}
			}
		}

		//initialize levels of image pyramid
		// 
		//initialize first level
		maxLevel = 1 + std::log(Max(resampledRes.x, resampledRes.y));
		pyramid.resize(maxLevel);
		pyramid.at(0).reset(new BlockedArray(resampledRes.x, resampledRes.y, resampledImage));

		int sRes = resampledRes.x;
		int tRes = resampledRes.y;
		for (int i = 1; i < maxLevel; i++) {
			//compute resolution and allocate memory for this image level
			sRes = Max(1, sRes / 2);
			tRes = Max(1, tRes / 2);
			pyramid.at(i).reset(new std::vector<T>(sRes * tRes));
			//filter finer level texel by a box filter,should be excuted synchronizedly
			for (int t = 0; t < tRes; t++) {
				for (int s = 0; s < sRes; s++) {
					(*pyramid[i])(s, t) = 0.25 * (texel(i - 1, s * 2, t * 2) + texel(i - 1, s * 2 + 1, t * 2)
						+ texel(i - 1, s * 2, t * 2 + 1) + texel(i - 1, s * 2 + 1, t * 2 + 1));
				}
			}
		}

	}

	//compute weights of original texels to resampled texels
	template<class T>
	std::vector<ResampleWeight> Mipmap<T>::resampleWeights(int32_t oldRes, int32_t newRes) {
		std::vector<ReSampleWeight> weights = std::vector<ReSampleWeight>(newRes);
		double filterWidth = 2.;
		for (int i = 0; i < newRes; i++) {
			//compute continuous texture coordinates of resampled texel relative to original sample coordinates
			double coord = (i + 0.5) * oldRes / newRes;

			//compute index of first texel contributes to new texel
			weights[i].firstTexel = std::floor(coord - filterWidth + 0.5);

			//compute weights of 4 adjcent texels
			for (int j = 0; j < 4; j++) {
				double pos = weights[i].firstTexel + j + 0.5;//convert discret coordinate to continuous coords 
				weights[i].weight[j] = lanczosSinc(abs(coord - pos));//compute weight using sinc function
			}

			//normalize weights so that they sum to 1
			double invSum = 1 / (weights[i].weight[0] + weights[i].weight[1]
				+ weights[i].weight[2] + weights[i].weight[3]);
			for (int j = 0; j < 4; j++)
				weights[i].weight[j] *= invSum;
		}
	}

	template<class T>
	T Mipmap<T>::lookup(const Point2f& st, double width)const {
		//compute continuous level according to width
		double level = maxLevel - 1 + std::log2(Max(width, 1e-8));
		if (level < 0)//return value of original picture at coordinate st
			return triangle(0, st);
		else if (level > maxLevel - 1)//maxLevel only has one texel
			return texel(maxLevel - 1, 0, 0);
		else {
			//triangle filter both image level beside and linear interpolate two filtered value
			int levelFlr = (int)std::floor(level);
			double delta = level - levelFlr;
			T vLastLevel = triangle(levelFlr, st);
			T vNextLevel = triangle(levelFlr + 1, st);
			return Lerp(delta, vLastLevel, vNextLevel);
		}
	}

	// perform triangle filter/bilinear interpolate 
	template<class T>
	T Mipmap<T>::triangle(int level, const Point2f& st)const {
		level = Clamp(level, 0, maxLevel - 1);
		//compute continous coordinate - 0.5 of sample point in order to compute distance
		float s = st[0] * pyramid[level]->uSize() - 0.5f;
		float t = st[1] * pyramid[level]->vSize() - 0.5f;

		int is = (int)std::floor(s);
		int it = (int)std::floor(t);
		//perform bilinear interpolate between four adjacent texels
		float ds = s - is;
		float dt = t - it;
		T sLerped0 = Lerp(ds, texel(level, is, it), texel(level, is + 1, it));
		T sLerped1 = Lerp(ds, texel(is, it + 1), texel(level, is + 1, it + 1));
		return Lerp(dt, sLerped0, sLerped1);
	}

	template<class T>
	const T& Mipmap<T>::texel(int level, int s, int t)const {
		//get image data of input level
		const BlockedArray& l = *pyramid[level];
		//bound st coordinates according to image wrap mode
		switch (wrapMode) {
		case ImRepeat:
			s = (s + l.uSize()) % l.uSize();
			t = (s + l.vSize()) % l.vSize();
			break;
		case ImClamp:
			s = Clamp(s, 0, l.uSize() - 1);
			t = Clamp(t, 0, l.vSize() - 1);
			break;
		case ImBlack:
			if (s<0 || s>l.uSize() - 1)return T(0.);
			if (t<0 || s>l.vSize() - 1)return T(0.);
			break;
		}
		return l(s, t);

	}

	template<class T>
	double Mipmap<T>::lanczosSinc(double v, double tau)const {
		v = v / tau;
		return sin(M_PI * v) / (M_PI * v);
	}
}