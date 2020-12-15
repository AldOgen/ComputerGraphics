#include "../libs/Camera.h"


CameraFly::CameraFly(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
    : zoom(ZOOM), yaw(yaw), pitch(pitch), position(position), world_up(up), front(FRONT) {
    UpdateCameraVectors();
}

CameraFly::CameraFly(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
    : zoom(ZOOM), yaw(yaw), pitch(pitch), position(glm::vec3(posX, posY, posZ)), world_up(glm::vec3(upX, upY, upZ)), front(FRONT) {
    UpdateCameraVectors();
}

glm::mat4 CameraFly::GetViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

GLfloat CameraFly::GetZoom() const {
    return zoom;
}

glm::vec3 CameraFly::GetPosition() const {
    return position;
}

void CameraFly::ProcessKeyboard(Movement direction, float deltaTime) {
    GLfloat velocity = SPEED * deltaTime;
    switch (direction) {
    case Movement::FORWARD:
        position += front * velocity;
        break;
    case Movement::BACKWARD:
        position -= front * velocity;
        break;
    case Movement::LEFT:
        position -= right * velocity;
        break;
    case Movement::RIGHT:
        position += right * velocity;
        break;
    default:
        std::cerr << "Undefined movement value" << std::endl;
        break;
    }
}

void CameraFly::ProcessMouseMovement(GLfloat x_offset, GLfloat y_offset, GLboolean constrain_pitch) {
    x_offset *= SENSITIVITY;
    y_offset *= SENSITIVITY;

    yaw += x_offset;
    pitch += y_offset;

    if (constrain_pitch) {
        if (pitch > PITCH_BORDER) {
            pitch = PITCH_BORDER;
        }
        if (pitch < -PITCH_BORDER) {
            pitch = -PITCH_BORDER;
        }
    }
    UpdateCameraVectors();
}

void CameraFly::ProcessMouseScroll(GLfloat yoffset) {
    if (zoom >= ZOOM_MIN_BORDER && zoom <= ZOOM_MAX_BORDER) {
        zoom -= yoffset;
    }
    if (zoom <= ZOOM_MIN_BORDER) {
        zoom = ZOOM_MIN_BORDER;
    }
    if (zoom >= ZOOM_MAX_BORDER) {
        zoom = ZOOM_MAX_BORDER;
    }
}

void CameraFly::UpdateCameraVectors() {
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    new_front.y = sin(glm::radians(pitch));
    new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(new_front);

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}
