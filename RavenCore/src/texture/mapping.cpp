#include<Raven/textute/mapping.h>

namespace Raven {

	std::tuple<Point2f, Vector2f, Vector2f> UVMapping2D::map(const SurfaceInteraction& si)const {

		Point2f st = Point2f(si.uv[0] * su + du, si.uv[1] * sv + dv);

		//su = dsdu, sv = dtdv
		//dsdx = dsdu * dudx, dtdx = dtdv * dvdx
		Vector2f dstdx = Vector2f(si.dudx * su, si.dvdx * sv);
		//dsdy = dsdu * dudy, dtdy = dtdv * dvdy
		Vector2f dstdy = Vector2f(si.dudy * su, si.dvdy * sv);

		return std::tuple<Point2f, Vector2f, Vector2f>(st, dstdx, dstdy);
	}

	std::tuple<Point2f, Vector2f, Vector2f> SphericalMapping2D::map(const SurfaceInteraction& si)const {
		Point2f st = mapSphere(si.p);

		//前向差分求偏导
		double delta = 0.1;

		Vector2f stDeltaX = mapSphere(si.p + delta * si.dpdx) - st;
		Vector2f dstdx = stDeltaX / delta;

		Vector2f stDeltaY = mapSphere(si.p + delta * si.dpdy) - st;
		Vector2f dstdy = stDeltaY / delta;

		return std::tuple<Point2f, Vector2f, Vector2f>(st, dstdx, dstdy);
	}

	Point2f SphericalMapping2D::mapSphere(const Point3f& p)const {
		Point3f localP = worldToTexture(p);

		//求该点在球面上的投影
		Vector3f vec = Normalize(localP - Point3f(0.0, 0.0, 0.0));

		//根据在球面的位置计算st
		double theta = SphericalTheta(vec) / M_PI;
		double phi = SphericalPhi(vec) / (M_PI * 2);
		return Point2f(theta, phi);
	}

	std::shared_ptr<UVMapping2D> makeUVMapping(const PropertyList& param) {
		double scaleU = param.getFloat("su");
		double scaleV = param.getFloat("sv");
		double uOffset = param.getFloat("du");
		double vOffset = param.getFloat("dv");
		return std::make_shared<UVMapping2D>(scaleU, scaleV, uOffset, vOffset);
	}

	std::shared_ptr<SphericalMapping2D> makeSphericalMapping2D(
		const std::shared_ptr<Transform>& worldTexture, const PropertyList& param) {
		return std::make_shared<SphericalMapping2D>(*worldTexture);
	}
}