#include<Raven/core/interaction.h>
#include<Raven/core/light.h>
namespace Raven {

	//根据微分光线计算交点的参数坐标(u,v)分别关于屏幕空间坐标(x,y)的偏导数
	void SurfaceInteraction::computeDifferential(const RayDifferential& rd) {
		//光线不含有微分光线，则纹理与法线的偏导数全部置零
		if (!rd.hasDifferential) {
			dudx = 0;
			dudy = 0;
			dvdx = 0;
			dvdy = 0;
			dpdx = Vector3f(0.0);
			dpdy = Vector3f(0.0);
		}
		else {
			//计算微分光线与场景的交点 px, py
			double d = -Dot(Vector3f(n), Vector3f(p));	
			double tx = (-Dot(n, Vector3f(rd.originX)) - d) / Dot(n, rd.directionX);
			Point3f px = rd.originX + rd.directionX * tx;
			double ty = (-Dot(n, Vector3f(rd.originY)) - d) / Dot(n, rd.directionY);
			Point3f py = rd.originY + rd.directionY * ty;

			//计算dpdx,dpdy,由于dx、dy为1,dpdx = dp,dpdy = dy
			dpdx = px - p;
			dpdy = py - p;

			//选择相关性较强的两个维度求解线性方程组
			int dim[2];
			if (std::abs(n.x) > std::abs(n.y) && std::abs(n.x) > std::abs(n.z)) {
				dim[0] = 1;
				dim[1] = 2;
			}
			else if (std::abs(n.y) > std::abs(n.z)) {
				dim[0] = 0;
				dim[1] = 2;
			}
			else {
				dim[0] = 0;
				dim[1] = 1;
			}
			double m[] = { dpdu[dim[0]],dpdv[dim[0]],dpdu[dim[1]],dpdv[dim[1]] };
			double bx[] = { dpdx[dim[0]],dpdx[dim[1]] };
			double by[] = { dpdy[dim[0]],dpdy[dim[1]] };

			//求解线性方程组解得dudx、dvdx、dudy、dvdy
			if (!solve2x2LinearSystem(m, bx, &dudx, &dvdx)) {
				dudx = 0;
				dvdx = 0;
			}
			if (!solve2x2LinearSystem(m, by, &dudy, &dvdy)) {
				dvdx = 0;
				dvdy = 0;
			}
		}
	}

	void SurfaceInteraction::SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv,
		const Normal3f& dndu, const Normal3f& dndv) {
		//compute shading.n 
		shading.n = Normalize(Cross(dpdu, dpdv));
		shading.n = FaceForward(n, shading.n);

		shading.dpdu = dpdu;
		shading.dpdv = dpdv;
		shading.dndu = dndu;
		shading.dndv = dndv;
	}

	Ray SurfaceInteraction::scartterRay(const Vector3f& dir)const {
		Point3f ori = p + dir * 0.001;
		return Ray(ori, dir);
	}

	Spectrum SurfaceInteraction::Le(const Vector3f& w)const{
		if (hitLight) {
			light->Li(*this, w);
		}
		else
			return Spectrum(0.0);
	}
}


