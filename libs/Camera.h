#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>


class CameraFly {
public:
    static enum class Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

public:
    CameraFly(glm::vec3 = POSITION, glm::vec3 = UP, GLfloat = YAW, GLfloat = PITCH);
    CameraFly(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat = YAW, GLfloat = PITCH);
    glm::mat4 GetViewMatrix() const;
    GLfloat GetZoom() const;
    glm::vec3 GetPosition() const;
    void ProcessKeyboard(Movement, float);
    void ProcessMouseMovement(GLfloat, GLfloat, GLboolean = true);
    void ProcessMouseScroll(GLfloat);

private:
    static constexpr GLfloat YAW = -90.0f;
    static constexpr GLfloat PITCH = 0.0f;
    static constexpr GLfloat PITCH_BORDER = 89.0f;

    static constexpr glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
    static constexpr glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr glm::vec3 FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

    static constexpr GLfloat SPEED = 0.005f;
    static constexpr GLfloat SENSITIVITY = 0.1f;
    static constexpr GLfloat ZOOM = 45.0f;
    static constexpr GLfloat ZOOM_MAX_BORDER = 45.0f;
    static constexpr GLfloat ZOOM_MIN_BORDER = 1.0f;

private:
    void UpdateCameraVectors();

private:
    GLfloat zoom;

    GLfloat pitch;
    GLfloat yaw;

    glm::vec3 position;
    glm::vec3 world_up;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
};