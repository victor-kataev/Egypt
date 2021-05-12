#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "params.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};


class Camera
{
public:
    glm::mat4 ProjectionMat;
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    bool Static;
    bool Movable;
    bool Flashlight;
    
    float Yaw;
    float Pitch;
    
    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), bool isStatic = false, bool isMovable = true, float yaw = YAW, float pitch = PITCH);
    void PlaceTo(const glm::vec3& position, float yaw, float pitch);
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ToggleStatic();
    glm::vec3 GetPosition() const;
    glm::vec3 GetFront() const;

private:
    void updateCameraVectors();
};