#ifndef _RAVEN_TEXTURE_MAPPING_2D_H_
#define _RAVEN_TEXTURE_MAPPING_2D_H_

#include"../core/texture.h"
#include"../core/base.h"


namespace Raven {

	/// <summary>
	/// UV映射，通过对UV坐标进行简单的仿射变换得到纹理坐标ST
	/// </summary>
	class UVMapping2D :public TextureMapping2D {
	public:
		UVMapping2D(double scaleU = 1.0, double scaleV = 1.0, double uOffset = 0.0, double vOffset = 0.0) {}

		virtual std::tuple<Point2f, Vector2f, Vector2f> map(const SurfaceInteraction& si)const;
	private:
		double su, sv;//UV坐标的缩放
		double du, dv;//UV坐标到偏移
	};

	/// <summary>
	/// SphericalMapping,通过建立包围球，将点映射到球面上生成纹理坐标ST
	/// </summary>
	class SphericalMapping2D :public TextureMapping2D {

	private:
		const std::shared_ptr<Transform> worldToTexture;
	};
}
#endif