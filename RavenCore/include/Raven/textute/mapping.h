#ifndef _RAVEN_TEXTURE_MAPPING_2D_H_
#define _RAVEN_TEXTURE_MAPPING_2D_H_

#include<Raven/core/texture.h>
#include<Raven/core/base.h>
#include<Raven/core/transform.h>
#include<Raven/utils/propertylist.h>

namespace Raven {

	/// <summary>
	/// UV映射，通过对UV坐标进行简单的仿射变换得到纹理坐标ST
	/// </summary>
	class UVMapping2D :public TextureMapping2D {
	public:
		UVMapping2D(double scaleU = 1.0, double scaleV = 1.0, double uOffset = 0.0, double vOffset = 0.0) :
			su(scaleU), sv(scaleV), du(uOffset), dv(vOffset) {}

		virtual std::tuple<Point2f, Vector2f, Vector2f> map(const SurfaceInteraction& si)const;

		static Ref<TextureMapping2D> construct(const PropertyList& param) {
			double su = param.getFloat("su", 1.0);
			double sv = param.getFloat("sv", 1.0);
			double du = param.getFloat("du", 0.0);
			double dv = param.getFloat("dv", 1.0);
			return std::make_shared<UVMapping2D>(su, sv, du, dv);
		}
	private:
		double su, sv;//UV坐标的缩放
		double du, dv;//UV坐标到偏移
	};


	/// <summary>
	/// SphericalMapping,通过建立包围球，将点映射到球面上生成纹理坐标ST
	/// </summary>
	class SphericalMapping2D :public TextureMapping2D {
	public:
		SphericalMapping2D(const Ref<Transform>& trans) :worldToTexture(trans) {}

		virtual std::tuple<Point2f, Vector2f, Vector2f> map(const SurfaceInteraction& si)const;

		static Ref<TextureMapping2D> construct(const PropertyList& param) {
			ObjectRef worldToTexture = param.getObjectRef(0);
			const auto& wtt = std::dynamic_pointer_cast<Transform>(worldToTexture.getRef());
			return std::make_shared<SphericalMapping2D>(wtt);
		}

	private:
		Point2f mapSphere(const Point3f& p)const;

		const Ref<Transform> worldToTexture;
	};
	_RAVEN_CLASS_REG_(uv, UVMapping2D, UVMapping2D::construct)

	_RAVEN_CLASS_REG_(spherical, SphericalMapping2D, SphericalMapping2D::construct)

}
#endif