// ui_button.c
#define GLFW_INCLUDE_NONE
#include "../include/glad/glad.h"
#include "ui_elements.h"
#include <stdlib.h>

void UIButton_Init(UIButton* btn) {
    // Define triangle vertices (simple right-facing play icon in NDC)
    float vertices[] = {
        0.875f, 0.925f,  // top-left
        0.875f, 0.875f,  // bottom-left
        0.925f, 0.90f   // middle-right
    };

    // Store bounds for click detection
    btn->x_min = vertices[0]; // -0.95f; 
    btn->x_max = vertices[1]; // -0.85f;
    btn->y_min = vertices[2]; // -0.95f;
    btn->y_max = vertices[4]; //-0.85f;

    // Setup VAO/VBO
    glGenVertexArrays(1, &btn->VAO);
    glGenBuffers(1, &btn->VBO);

    glBindVertexArray(btn->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, btn->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void UIButton_Render(const UIButton* btn) {
    glBindVertexArray(btn->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

int UIButton_IsClicked(const UIButton* btn, float x_ndc, float y_ndc) {
    return (x_ndc >= btn->x_min && x_ndc <= btn->x_max &&
            y_ndc >= btn->y_min && y_ndc <= btn->y_max);
}

void UIButton_Destroy(UIButton* btn) {
    glDeleteVertexArrays(1, &btn->VAO);
    glDeleteBuffers(1, &btn->VBO);
}