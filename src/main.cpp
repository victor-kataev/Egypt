#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "scene.h"
#include "params.h"


float deltaTime = 0;
float lastFrame = 0;

//scene.GetCamera() scene.GetCamera()(glm::vec3(0.0, 0.0, 3.0));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
bool disabled = true;
//bool pressed_before = false;

glm::vec3 g_LightPos(1.0f, 1.0f, 1.3f);
float yPos = 0.0f;

bool flashlightPressed = false;
bool cameraStaticPressed = false;

Scene scene;


void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadTexture(const char* texPath);
void init();
GLFWwindow* createWindow(int width, int height, const std::string title);
void setUpWindow(GLFWwindow* window);
void processInput(GLFWwindow* window);

void OnRender();
//void OnUIRender();


int main()
{
    init();
    GLFWwindow* window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Egypt");
    glfwMakeContextCurrent(window);

    bool err = glewInit() != GLEW_OK;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        exit(-1);
    }
    
    setUpWindow(window);
    glEnable(GL_DEPTH_TEST);

    scene.Init();


    //render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        OnRender();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();


    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(FORWARD, deltaTime * 10);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(FORWARD, deltaTime * 0.1);
        else
            scene.GetCamera()->ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(BACKWARD, deltaTime * 10);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(BACKWARD, deltaTime * 0.1);
        else
            scene.GetCamera()->ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(LEFT, deltaTime * 10);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(LEFT, deltaTime * 0.1);
        else
            scene.GetCamera()->ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(RIGHT, deltaTime * 10);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(RIGHT, deltaTime * 0.1);
        else
            scene.GetCamera()->ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(UP, deltaTime * 10);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(UP, deltaTime * 0.1);
        else
            scene.GetCamera()->ProcessKeyboard(UP, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(DOWN, deltaTime * 10);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            scene.GetCamera()->ProcessKeyboard(DOWN, deltaTime * 0.1);
        else
            scene.GetCamera()->ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        scene.SetMainCamera(0);

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        scene.SetMainCamera(1);

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        scene.SetMainCamera(2);
    
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        scene.SetMainCamera(3);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        yPos += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        yPos -= 0.1f;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        flashlightPressed = true;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && flashlightPressed)
    {
        scene.ToggleFlashlight();
        flashlightPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        cameraStaticPressed = true;
    
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && cameraStaticPressed)
    {
        scene.GetCamera()->ToggleStatic();
        cameraStaticPressed = false;
    }


}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    scene.GetCamera()->ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void init()
{
    if (!glfwInit())
        exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* createWindow(int width, int height, const std::string title)
{
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create glfw window\n";
        glfwTerminate();
        exit(-1);
    }

    return window;
}

void setUpWindow(GLFWwindow* window)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void OnRender()
{
    glClearColor(0.2, 0.3, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.Render();
}
