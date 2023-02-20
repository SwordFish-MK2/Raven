#ifndef _RAVEN_CORE_CAMERA_H_
#define _RAVEN_CORE_CAMERA_H_

#include <Raven/core/base.h>
#include <Raven/core/math.h>
#include <Raven/core/object.h>
#include <Raven/core/transform.h>

#include <optional>

namespace Raven {

class Camera : public RavenObject {
 public:
  Camera(const Transform& CTW, std::unique_ptr<Film> f, Ref<Medium> medium)
      : CameraToWorld(CTW),medium(medium) {
        film=std::move(f);
      }

  virtual std::optional<Ray> generateRay(const CameraSample& sample) const = 0;
  virtual std::optional<RayDifferential> generateRayDifferential(
      const CameraSample& sample) const = 0;

 protected:
  Transform CameraToWorld;  // place the camera in the scene, including rotation
                            // and translation

 public:
  std::unique_ptr<Film> film;  // 相机的幕布
  Ref<Medium> medium;  // 相机所处的介质，空指针对应无介质
};
}  // namespace Raven
#endif