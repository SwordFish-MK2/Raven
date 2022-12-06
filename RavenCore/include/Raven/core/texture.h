#ifndef _RAVEN_CORE_TEXTURE_H_
#define _RAVEN_CORE_TEXTURE_H_

#include<Raven/core/base.h>
#include<Raven/core/interaction.h>
#include<Raven/utils/propertylist.h>
#include<Raven/core/object.h>
#include<Raven/utils/factory.h>

namespace Raven {
	/// <summary>
	/// 2D纹理mapping 
	/// </summary>
	class TextureMapping2D :public RavenObject {
	public:
		virtual std::tuple<Point2f, Vector2f, Vector2f> map(const SurfaceInteraction& si)const = 0;
	};

	/// <summary>
	/// 3D纹理mapping
	/// </summary>
	class TextureMapping3D :public RavenObject {
	public:
		virtual std::tuple<Point3f, Vector3f, Vector3f>map(const SurfaceInteraction& si)const = 0;
	};

	/// <summary>
	/// 纹理接口
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <class T>
	class Texture :public RavenObject {
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
	private:
		T value;
	};

	template<class T>
	class ScaleTexture :public Texture<T> {
	public:
		virtual T evaluate(const SurfaceInteraction& its)const {
			return vTexuture->evaluate(its) * sTexture->evaluate(its);
		}
		ScaleTexture(const Ref<Texture<T>>& t1, const Ref<Texture<T>>& t2) :vTexture(t1), sTexture(t2) {}
	private:
		Ref<Texture<T>> vTexture;
		Ref<Texture<T>> sTexture;
	};

	namespace TextureBuild {
		Ref<Texture<double>> makeConstTextureFloat(const PropertyList& param);
		Ref<Texture<Spectrum>> makeConstTextureSpectrum(const PropertyList& param);

		Ref<Texture<double>> makeScaleTextureFloat(const PropertyList& param);
		Ref<Texture<Spectrum>> makeScaleTextureSpectrum(const PropertyList& param);
	}

	_RAVEN_CLASS_REG_(constfloat,ConstTextureFloat,TextureBuild::makeConstTextureFloat)

	_RAVEN_CLASS_REG_(constspectra,ConstTextureSpectra,TextureBuild::makeConstTextureSpectrum)

	_RAVEN_CLASS_REG_(scalefloat,ScaleTextureFloat,TextureBuild::makeScaleTextureFloat)

	_RAVEN_CLASS_REG_(scalespectra,ScaleTextureSpectra,TextureBuild::makeScaleTextureSpectrum)
}


#endif
