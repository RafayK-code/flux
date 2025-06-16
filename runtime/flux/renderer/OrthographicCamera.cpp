#include <pch.h>

#include <flux/renderer/OrthographicCamera.h>

#include <glm/gtc/matrix_transform.hpp>

namespace flux
{
    OrthographicCamera::OrthographicCamera(float width, float height, float near, float far)
        : position_(glm::vec3(0.0f, 0.0f, 0.0f))
        , rotation_(0.0f)
        , zoom_(1.0f)
        , projectionMatrix_(glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, near, far))
    {
        UpdateViewMatrix();
    }

    void OrthographicCamera::UpdateViewMatrix()
    {
        glm::mat4 transform =
            glm::translate(glm::mat4(1.0f), position_) *
            glm::rotate(glm::mat4(1.0f), rotation_, glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / zoom_, 1.0f / zoom_, 1.0f));

        viewMatrix_ = glm::inverse(transform);
        viewProjectionMatrix_ = projectionMatrix_ * viewMatrix_;
    }
}