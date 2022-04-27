#include"interaction.h"

namespace Raven {
	void SurfaceInteraction::computeDifferential(const RayDifferential& rd) {
		if (!rd.hasDifferential) {
			dudx = 0;
			dudy = 0;
			dvdx = 0;
			dvdy = 0;
			dpdx = Vector3f(0.0);
			dpdy = Vector3f(0.0);
		}
		else {
			//compute coordinate of px, py
			double d = -Dot(Vector3f(n), Vector3f(p));
			double tx = (-Dot(n, Vector3f(rd.originX)) - d) / Dot(n, rd.directionX);
			Point3f px = rd.originX + rd.directionX * tx;
			double ty = (-Dot(n, Vector3f(rd.originY)) - d) / Dot(n, rd.directionY);
			Point3f py = rd.originY + rd.directionY * ty;

			//compute dpdx dpdy
			dpdx = px - p;
			dpdy = py - p;

			//choose two dimention to solve the linear equation system
			int dim[2];
			if (n.x > n.y && n.x > n.z) {
				dim[0] = 1;
				dim[1] = 2;
			}
			else if (n.y > n.z && n.y > n.x) {
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

			//solve linear system to get partial derivitive of uv over xy
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

}