#ifndef UI_BUTTON_H
#define UI_BUTTON_H
#include <GL/gl.h>


typedef struct {
    float x_min, x_max, y_min, y_max;  // NDC bounds of the button
    GLuint VAO, VBO;                   // OpenGL handles for rendering
} UIButton;

// Initialize a triangular play button at specified NDC bounds
void UIButton_Init(UIButton* btn);

// Render the button using the currently bound shader
void UIButton_Render(const UIButton* btn);

// Check if a mouse click at (x_ndc, y_ndc) is inside the button
int UIButton_IsClicked(const UIButton* btn, float x_ndc, float y_ndc);

// Clean up OpenGL resources
void UIButton_Destroy(UIButton* btn);

#endif