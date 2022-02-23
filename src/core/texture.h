#ifndef _RAVEN_CORE_TEXTURE_H_
#define _RAVEN_CORE_TEXTURE_H_

#include"base.h"
#include"interaction.h"

namespace Raven {
	/// <summary>
	/// Store object surface texture 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <class T>
	class Texture {
	public:
		virtual T evaluate(const SurfaceInteraction& its)const = 0;
	};

	/// <summary>
	/// Const color
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <class T>
	class ConstTexture :public Texture<T> {
	public:
		ConstTexture(T value) :value(value) {}
		virtual T evaluate(const SurfaceInteraction& its)const {
			return value;
		}
	private:
		T value;
	};
	template<class T>
	class ScaleTexture :public Texture<T> {
	public:
		virtual T evaluate(const SurfaceInteraction& its)const {
			return vTexuture->evaluate(its) * sTexture->evaluate(its);
		}
	private:
		std::shared_ptr<Texture<T>> vTexuture;
		std::shared_ptr<Texture<T>> sTexture;
	};
}

#endif