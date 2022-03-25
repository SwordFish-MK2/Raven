#ifndef _RAVEN_CORE_TEXTURE_H_
#define _RAVEN_CORE_TEXTURE_H_

#include"base.h"
#include"interaction.h"

namespace Raven {
	/// <summary>
	/// 2D纹理mapping 
	/// </summary>
	class TextureMapping2D {
		virtual std::tuple<Point2f, Vector2f, Vector2f> map(const SurfaceInteraction& si)const = 0;
	};

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
	/// Const Texture,对所有的纹理坐标返回一个常量
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <class T>
	class ConstTexture :public Texture<T> {
	public:
		ConstTexture(T value) :value(value) {}
		virtual  inline T evaluate(const SurfaceInteraction& its)const {
			return value;
		}
		static std::shared_ptr<ConstTexture<T>> build(const T& value) {
			return std::make_shared<ConstTexture<T>>(value);
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
		ScaleTexture(const std::shared_ptr<Texture<T>>& t1, const std::shared_ptr<Texture<T>>& t2) :vTexture(t1), sTexture(t2) {}
		static std::shared_ptr<ScaleTexture<T>> build(const std::shared_ptr<Texture<T>>& vT, const std::shared_ptr<Texture<T>>& sT);
	private:
		std::shared_ptr<Texture<T>> vTexuture;
		std::shared_ptr<Texture<T>> sTexture;
	};
}

#endif