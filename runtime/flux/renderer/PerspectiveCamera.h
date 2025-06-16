#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace flux
{
    class PerspectiveCamera : public RefCounted
    {
    public:
        PerspectiveCamera(float fov, float aspectRatio, float near, float far);
        ~PerspectiveCamera() = default;

        inline const glm::vec3& Position() const { return position_; }
        inline void SetPosition(const glm::vec3& position) { position_ = position; UpdateViewMatrix(); }

        inline const glm::quat& Orientation() const { return orientation_; }
        inline void SetOrientation(const glm::quat& orientation) { orientation_ = orientation; }

        void SetRotation(const glm::vec3& eulerAngle);
        inline void SetRotation(const glm::quat& rotation) { orientation_ = glm::normalize(rotation * orientation_); UpdateViewMatrix(); }

        void LookAt(const glm::vec3& target, const glm::vec3& up);

        inline const glm::mat4& ViewMatrix() const { return viewMatrix_; }
        inline const glm::mat4& ProjectionMatrix() const { return projectionMatrix_; }
        inline const glm::mat4& ViewProjectionMatrix() const { return viewProjectionMatrix_; }

    private:
        void UpdateViewMatrix();

    private:
        glm::vec3 position_;
        glm::quat orientation_;

        glm::mat4 viewMatrix_;
        glm::mat4 projectionMatrix_;
        glm::mat4 viewProjectionMatrix_;
    };
}