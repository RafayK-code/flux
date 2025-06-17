#include <fxpch.h>

#include <flux/renderer/PerspectiveCamera.h>
#include <glm/gtx/quaternion.hpp>

namespace flux
{
    PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float near, float far)
        : position_(glm::vec3(0.0f, 0.0f, 0.0f))
        , orientation_(glm::quat())
        , projectionMatrix_(glm::perspective(fov, aspectRatio, near, far))
    {
        UpdateViewMatrix();
    }

    void PerspectiveCamera::SetRotation(const glm::vec3& eulerAngle)
    {
        glm::quat pitchQuat = glm::angleAxis(eulerAngle.x, glm::vec3(1, 0, 0));
        glm::quat yawQuat = glm::angleAxis(eulerAngle.y, glm::vec3(0, 1, 0));
        glm::quat rollQuat = glm::angleAxis(eulerAngle.z, glm::vec3(0, 0, 1));

        orientation_ = glm::normalize(yawQuat * pitchQuat * rollQuat);
        UpdateViewMatrix();
    }

    void PerspectiveCamera::LookAt(const glm::vec3& target, const glm::vec3& up)
    {
        glm::vec3 forward = glm::normalize(target - position_);
        orientation_ = glm::quatLookAt(forward, up);
        UpdateViewMatrix();
    }

    void PerspectiveCamera::UpdateViewMatrix()
    {
        viewMatrix_ = glm::toMat4(glm::conjugate(orientation_)) * glm::translate(glm::mat4(1.0f), -position_);
        viewProjectionMatrix_ = projectionMatrix_ * viewMatrix_;
    }
}