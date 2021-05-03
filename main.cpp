#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <iostream>

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb/stb_image.h"

#include "shader.h"
#include "camera.h"
#include "model.h"
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


Scene scene;


void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadTexture(const char* texPath);
void init();
GLFWwindow* createWindow(int width, int height, const std::string title);
void setUpWindow(GLFWwindow* window);
void processInput(GLFWwindow* window);

void OnRender();
void OnUIRender();


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

    
#if 0
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    Shader lightingShader("vertex.glsl", "fragment.glsl");
    Shader lightCubeShader("vertex_light_source.glsl", "fragment_light_source.glsl");

    unsigned int cubeVAO, lightCubeVAO;
    unsigned int VBO;

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);


    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    unsigned int diffuseMap = loadTexture("container2.png");
    unsigned int specularMap = loadTexture("container2_specular.png");
    unsigned int emissionMap = loadTexture("matrix.jpg");

    lightingShader.use();
    lightingShader.setInt("material.texture_diffuse1", 0);
    lightingShader.setInt("material.texture_specular1", 1);
   // lightingShader.setInt("material.emission", 2);


    Model backpack("models/egypt/pyramids.obj");

    glm::vec3 modelPositions[] = {
        glm::vec3(5.0f, 5.0f, 5.0f),
        glm::vec3(-3.0f, -3.0f, -3.0f)
    };
#endif

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float time = currentFrame;
#if 0
        float time = currentFrame;
        float xPos = sin(time);
        float zPos = cos(time);
        xPos *= 3;
        zPos *= 3;
        g_LightPos = glm::vec3(xPos, yPos, zPos);
       
        glm::vec3 lightColor(1.0f);
        //lightColor.x = sin(time * 2.0);
        //lightColor.y = sin(time * 0.7);
        //lightColor.z = sin(time * 1.3);
#endif

        processInput(window);
        OnRender();
        
#if 0
        glClearColor(0.2, 0.3, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.f);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = scene.GetCamera()->GetViewMatrix();

        model = glm::mat4(1.0f);
        model = glm::translate(model, g_LightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("model", model);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setVec3("myColor", lightColor);


        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);

        //lightingShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        //lightingShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setVec3("lightPos", g_LightPos);
        lightingShader.setVec3("viewerPos", scene.GetCamera()->Position);

        //lightingShader.setVec3("material.ambient", materials.jade.ambient);
        //lightingShader.setVec3("material.diffuse", materials.jade.diffuse);
        //lightingShader.setVec3("material.specular", materials.jade.specular);
        lightingShader.setFloat("material.shininess", 32.0);

        
        glm::vec3 ambientColor = lightColor * glm::vec3(0.2);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5);
        lightingShader.setVec3("light.ambient", ambientColor);
        lightingShader.setVec3("light.diffuse", diffuseColor);
        lightingShader.setVec3("light.specular", lightColor);

        /*for (int i = 0; i < 2; i++)
        {
            model = glm::mat4(1.0);
            model = glm::translate(model, modelPositions[i]);
            if(i == 1)
                model = glm::scale(model, glm::vec3(0.8, 1.0, 1.0));
            lightingShader.setMat4("model", model);
        }*/
        backpack.Draw(lightingShader);

        //lightingShader.setFloat("time", time);

        /*model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);*/
        
#endif
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#if 0
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
#endif
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

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        yPos += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        yPos -= 0.1f;
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

//unsigned int loadTexture(const char* texPath)
//{
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//
//    int width, height, nrComponents;
//    unsigned char* data = stbi_load(texPath, &width, &height, &nrComponents, 0);
//    if (data)
//    {
//        GLenum format;
//        if (nrComponents == 1)
//            format = GL_RED;
//        else if (nrComponents == 3)
//            format = GL_RGB;
//        else if (nrComponents == 4)
//            format = GL_RGBA;
//
//        glBindTexture(GL_TEXTURE_2D, textureID);
//        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//        stbi_image_free(data);
//    }
//    else
//    {
//        std::cout << "Texture failed to load at path: " << texPath << std::endl;
//        stbi_image_free(data);
//    }
//
//    return textureID;
//}

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