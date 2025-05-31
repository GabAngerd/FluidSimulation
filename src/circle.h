#ifndef CIRCLE_H
#define CIRCLE_H

typedef struct {
    float xPos;
    float yPos;
    float xVelocity;
    float yVelocity;
    float radius;
    unsigned int VAO; // Vertex Array Object
    unsigned int VBO; // Vertex Buffer Object
} Circle;

#endif // CIRCLE_H