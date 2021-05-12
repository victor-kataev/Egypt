#include "camera.h"

Camera::Camera(glm::vec3 position, bool isStatic, bool isMovable, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    WorldUp(0.0f, 1.0f, 0.0f),
    Static(isStatic),
    Movable(isMovable),
    Flashlight(false)
{
    ProjectionMat = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1500.f);
    Position = position;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

void Camera::PlaceTo(const glm::vec3& position, float yaw, float pitch)
{
    Position = position;
    Yaw = yaw;
    Pitch = pitch;
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix()
{
    return ProjectionMat;
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    if (Static || !Movable)
        return;

    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if (direction == UP)
        Position += WorldUp * velocity;
    if (direction == DOWN)
        Position -= WorldUp * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    if (Static)
        return;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ToggleStatic()
{
    Static = !Static;

}

glm::vec3 Camera::GetPosition() const
{
    return Position;
}

glm::vec3 Camera::GetFront() const
{
    return Front;
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
