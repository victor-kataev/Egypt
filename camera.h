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
    
    float Yaw;
    float Pitch;
    
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;


    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), bool isStatic = false, bool isMovable = true, float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Zoom(ZOOM),
        WorldUp(0.0f, 1.0f, 0.0f),
        Static(isStatic),
        Movable(isMovable)
    {
        ProjectionMat = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1500.f);
        Position = position;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }


    void PlaceTo(const glm::vec3& position, float yaw, float pitch)
    {
        Position = position;
        Yaw = yaw;
        Pitch = pitch;
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 GetProjectionMatrix()
    {
        return ProjectionMat;
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
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

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
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

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    void ToggleStatic()
    {
        Static = !Static;
    }

    glm::vec3 GetPosition() const
    {
        return Position;
    }

    glm::vec3 GetFront() const
    {
        return Front;
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};