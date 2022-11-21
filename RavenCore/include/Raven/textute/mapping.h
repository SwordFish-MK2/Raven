#ifndef _RAVEN_TEXTURE_MAPPING_2D_H_
#define _RAVEN_TEXTURE_MAPPING_2D_H_

#include<Raven/core/texture.h>
#include<Raven/core/base.h>
#include<Raven/core/transform.h>
#include<Raven/utils/propertylist.h>

namespace Raven {

	/// <summary>
	/// UVӳ�䣬ͨ����UV������м򵥵ķ���任�õ���������ST
	/// </summary>
	class UVMapping2D :public TextureMapping2D {
	public:
		UVMapping2D(double scaleU = 1.0, double scaleV = 1.0, double uOffset = 0.0, double vOffset = 0.0) :
			su(scaleU), sv(scaleV), du(uOffset), dv(vOffset) {}

		virtual std::tuple<Point2f, Vector2f, Vector2f> map(const SurfaceInteraction& si)const;

		static std::shared_ptr<UVMapping2D> build(double su = 1.0, double sv = 1.0, double du = 0.0, double dv = 0.0) {
			return std::make_shared<UVMapping2D>(su, sv, du, dv);
		}
	private:
		double su, sv;//UV���������
		double du, dv;//UV���굽ƫ��
	};


	/// <summary>
	/// SphericalMapping,ͨ��������Χ�򣬽���ӳ�䵽������������������ST
	/// </summary>
	class SphericalMapping2D :public TextureMapping2D {
	public:
		SphericalMapping2D(const Transform& trans) :worldToTexture(trans) {}

		virtual std::tuple<Point2f, Vector2f, Vector2f> map(const SurfaceInteraction& si)const;

		static std::shared_ptr<SphericalMapping2D> build(const Transform& trans) {
			return std::make_shared<SphericalMapping2D>(trans);
		}
	private:
		const Transform worldToTexture;
		Point2f mapSphere(const Point3f& p)const;
	};
	
	std::shared_ptr<UVMapping2D> makeUVMapping(const PropertyList& param);

	std::shared_ptr<SphericalMapping2D> makeSphericalMapping2D(
		const std::shared_ptr<Transform>& worldTexture, const PropertyList& param);
}
#endif