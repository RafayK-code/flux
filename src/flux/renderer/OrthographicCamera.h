#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <glm/glm.hpp>

namespace flux
{
    /**
    * Technically this isnt a true orthographic camera. Rather it is an orthgraphic camera from a fixed perspective.
    * 
    * If we wanted this to be a true camera, we would need to have better support for rotation along the x and y axis's. However generally speaking
    * allowing that for an orthographic camera just looks really fucked up and funky. We probably will keep this as a pseudo-camera as
    * it makes far more sense to
    */
    class OrthographicCamera : public RefCounted
    {
    public:
        OrthographicCamera(float width, float height, float near, float far);
        ~OrthographicCamera() = default;

        inline const glm::vec3& Position() const { return position_; }
        inline void SetPosition(const glm::vec3& position) { position_ = position; UpdateViewMatrix(); }

        inline float Rotation() const { return rotation_; }
        inline void SetRotation(float rotation) { rotation_ = rotation; UpdateViewMatrix(); }

        inline float Zoom() const { return zoom_; }
        inline void SetZoom(float zoom) { zoom_ = zoom; UpdateViewMatrix(); }

        inline const glm::mat4& ViewMatrix() const { return viewMatrix_; }
        inline const glm::mat4& ProjectionMatrix() const { return projectionMatrix_; }
        inline const glm::mat4& ViewProjectionMatrix() const { return viewProjectionMatrix_; }

    private:
        void UpdateViewMatrix();

    private:
        glm::vec3 position_;
        float rotation_;
        float zoom_;

        glm::mat4 viewMatrix_;
        glm::mat4 projectionMatrix_;
        glm::mat4 viewProjectionMatrix_;
    };
}