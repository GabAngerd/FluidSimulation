#define GLFW_INCLUDE_NONE
#include "../include/glad/glad.h"
#include "ui_elements.h"
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> // For sin and cos functions

#define M_PI 3.14159265358979323846

// Vertex Shader source code
const char* vertexShaderSource = "#version 460 core\n"
    "layout(location = 0) in vec2 aPos;\n"
    "uniform vec2 offset;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos + offset, 0.0, 1.0);\n"
    "}\0";

// Fragment Shader source code
const char* fragmentShaderSource = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n" // White color
    "}\0";


UIButton playButton;
int animationPlaying = 0;

// Function to compile shader and check errors
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check compile errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed:\n%s\n", infoLog);
        exit(EXIT_FAILURE);
    }
    return shader;
}

// Create a shader program
GLuint createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "Shader linking failed:\n%s\n", infoLog);
        exit(EXIT_FAILURE);
    }

    // shaders no longer needed after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get cursor position in window coordinates (top-left origin)
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Get window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Convert window coordinates to NDC (-1 to 1), with (0,0) at center
        float x_ndc = (float)((xpos / width) * 2.0 - 1.0);
        float y_ndc = (float)(1.0 - (ypos / height) * 2.0); // flip y axis

        // Check if click is inside the play button's area
        if (UIButton_IsClicked(&playButton, x_ndc, y_ndc)) {
            animationPlaying = !animationPlaying;
            printf("Animation %s\n", animationPlaying ? "started" : "stopped");
        }
    }
}

// Generate circle vertex positions
void generateCircleVertices(float* vertices, float centerX, float centerY, float radius, int numSegments) {
    vertices[0] = centerX;
    vertices[1] = centerY;
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0f * M_PI * i / numSegments;
        vertices[2 * (i + 1)] = centerX + radius * cosf(theta);
        vertices[2 * (i + 1) +1] = centerY + radius * sinf(theta);
    }
}


int main(void) {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //Set the version to 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);


    GLFWwindow* window = glfwCreateWindow(940, 880, "FLUIDSIMULATIONS", NULL, NULL); //Create window
    // Handle if for some reason window does not work
    if (!window) { 
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); // Make the OpenGL context current

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compile shaders and create shader program
    GLuint shaderProgram = createShaderProgram();

    GLint offsetLocation = glGetUniformLocation(shaderProgram, "offset");


    UIButton_Init(&playButton);




    // Circle parameters
    int numSegments = 100;
    float centerX = 0.0f;
    float centerY = 0.0f;
    float radius = 0.01f;

    float* circleVertices = malloc(sizeof(float) * 2 * numSegments);
    if (!circleVertices) {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }
    generateCircleVertices(circleVertices, centerX, centerY, radius, numSegments);

    // Generate VAO and VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind and fill VBO with circle vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numSegments, circleVertices, GL_STATIC_DRAW);

    // Setup vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO (optional but good practice)
    glBindVertexArray(0);

    free(circleVertices);    

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);
        float xPos = 0.0f;
        float yPos = 0.0f;

        if (animationPlaying) {
            float time = (float)glfwGetTime();
            xPos = 0.5f * sinf(time);
            yPos = 0.5f * cosf(time);
        }
        // Use shader program and bind VAO
        glUseProgram(shaderProgram);
        glUniform2f(offsetLocation, xPos, yPos);
        glBindVertexArray(VAO);

        // Draw circle as line loop
        glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);
        glUniform2f(offsetLocation, 0.0f, 0.0f);
        UIButton_Render(&playButton);
        // Unbind VAO
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }



    //clean up
    UIButton_Destroy(&playButton);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
