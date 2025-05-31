#define GLFW_INCLUDE_NONE
#include "../include/glad/glad.h"
#include "ui_elements.h"
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> // For sin and cos functions
#include "physics.h"
#include "circle.h"

#define M_PI 3.14159265358979323846
#define MAX_CIRCLES 1000
#define CIRCLE_NBR_SEGMENTS 100
#define GRID_LENGTH 100

//typedef struct {
//    float xPos;
//    float yPos;
//    float radius;
//    float xVelocity;
//    float yVelocity;
//    GLuint VAO;
//    GLuint VBO;
//} Circle;


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

Circle circles[MAX_CIRCLES];

void initializeCircles() {
    float spacing = 0.004f;

    float* circleVertices = malloc(sizeof(float) * 2 * CIRCLE_NBR_SEGMENTS * MAX_CIRCLES);
    if (!circleVertices) {
        fprintf(stderr, "Failed to allocate memory for circle\n");
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i < MAX_CIRCLES; i++) {
        int row = i / GRID_LENGTH;
        int col = i % GRID_LENGTH;
        float offset = (MAX_CIRCLES/GRID_LENGTH )* spacing*2;
    

        circles[i].xPos = col * spacing - offset ;//(float)(i * 0.1f); // Example positions
        circles[i].yPos = row * spacing; //(float)(i * 0.1f);

        circles[i].radius = 0.007f;
        circles[i].xVelocity = 0.5f * ((float)rand()/RAND_MAX);
        circles[i].yVelocity = 0.01f * ((float)rand()/RAND_MAX);

        // Generate circle vertices
        //int numSegments = 100;
        
        generateCircleVertices(&circleVertices[i * 2 * CIRCLE_NBR_SEGMENTS], 0.0f, 0.0f, circles[i].radius, CIRCLE_NBR_SEGMENTS);
    }
    // Generate VAO and VBO
    glGenVertexArrays(1, &circles[0].VAO);
    glGenBuffers(1, &circles[0].VBO);

    glBindVertexArray(circles[0].VAO);
    glBindBuffer(GL_ARRAY_BUFFER, circles[0].VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * CIRCLE_NBR_SEGMENTS *MAX_CIRCLES, circleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    free(circleVertices);
    
}

void renderCircles(GLuint shaderProgram, GLint offsetLocation) {
    glUseProgram(shaderProgram);
    glBindVertexArray(circles[0].VAO); // Bind the single VAO

    for (int i = 0; i < MAX_CIRCLES; i++) {
        glUniform2f(offsetLocation, circles[i].xPos, circles[i].yPos);
        glDrawArrays(GL_TRIANGLE_FAN, i * (CIRCLE_NBR_SEGMENTS + 2), CIRCLE_NBR_SEGMENTS + 2); // Draw each circle
    }

    glBindVertexArray(0);
}

int main(void) {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x multisampling

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

    glEnable(GL_MULTISAMPLE); // Enable multisampling


    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compile shaders and create shader program
    GLuint shaderProgram = createShaderProgram();

    GLint offsetLocation = glGetUniformLocation(shaderProgram, "offset");


    UIButton_Init(&playButton);
    
    initializeCircles();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    float timestep = 0.05f;


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Render circles if animation is playing
        if (animationPlaying) {
            updatePosition(circles, MAX_CIRCLES, timestep);//(&circles[i].xPos, &circles[i].yPos, &circles[i].xVelocity, &circles[i].yVelocity, timestep);


            renderCircles(shaderProgram,offsetLocation);
            //for (int i = 0; i < MAX_CIRCLES; i++) {
            //    glUniform2f(offsetLocation, circles[i].xPos, circles[i].yPos);
            //    glBindVertexArray(circles[i].VAO);
            //    glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NBR_SEGMENTS + 2); // numSegments + 2
            //    glBindVertexArray(0);
            //}
        }



        // Render play button (static, unaffected by animation)
        glBindVertexArray(0); // Unbind any VAOs
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
