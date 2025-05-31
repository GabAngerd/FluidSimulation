#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "circle.h"

#define ACC_GRAVITY 1
#define WINDOW_BOTTOM -1.0f  // Bottom boundary of the window
#define WINDOW_TOP 1.0f 
#define WINDOW_LEFT -1.0f  // Bottom boundary of the window
#define WINDOW_RIGHT 1.0f 

static bool checkCollision(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distanceSquared = dx * dx + dy * dy;
    float radiusSum = r1 + r2;
    return distanceSquared < radiusSum * radiusSum; // Check if distance is less than the sum of radii
}

static void resolveCollision(float* x1, float* y1, float* vx1, float* vy1, float r1,
    float* x2, float* y2, float* vx2, float* vy2, float r2) {
    float dx = *x2 - *x1;
    float dy = *y2 - *y1;
    float distance = sqrtf(dx * dx + dy * dy);

    if (distance == 0.0f) return; // Avoid division by zero

    // Normalize the direction vector
    float nx = dx / distance;
    float ny = dy / distance;

    // Separate the circles
    float overlap = r1 + r2 - distance;
    *x1 -= nx * overlap / 2.0f;
    *y1 -= ny * overlap / 2.0f;
    *x2 += nx * overlap / 2.0f;
    *y2 += ny * overlap / 2.0f;

    // Reflect velocities (simple collision response)
    float dotProduct1 = *vx1 * nx + *vy1 * ny;
    float dotProduct2 = *vx2 * nx + *vy2 * ny;

    *vx1 -= 1.96f * dotProduct1 * nx;
    *vy1 -= 1.96f * dotProduct1 * ny;
    *vx2 -= 1.96f * dotProduct2 * nx;
    *vy2 -= 1.96f * dotProduct2 * ny;
}

void updatePosition(Circle* circles, int NumCircles, float timestep) {//(float* xPos,float* yPos,float* xVelocity,float* yVelocity, float timestep){
    //*xPos = 0.5f * sinf(time);
    //*yPos = 0.5f * cosf(time);

    //_includeGravity(yPos, yVelocity,timestep);
    for (int i = 0; i < NumCircles; i++) {
        Circle* c1 = &circles[i];

        c1->xPos += c1->xVelocity * timestep;
        c1->yPos += c1->yVelocity * timestep;

        c1->yVelocity -= ACC_GRAVITY * timestep; //include gravity


        if (c1->yPos <= WINDOW_BOTTOM) {
            c1->yPos = WINDOW_BOTTOM;
            c1->yVelocity = -c1->yVelocity;
        } else if (c1->yPos >= WINDOW_TOP) {
            c1->yPos = WINDOW_TOP;
            c1->yVelocity = -c1->yVelocity;
        }

        if (c1->xPos <= WINDOW_LEFT) {
            c1->xPos = WINDOW_LEFT;
            c1->xVelocity = -c1->xVelocity;
        } else if (c1->xPos >= WINDOW_RIGHT) {
            c1->xPos = WINDOW_RIGHT;
            c1->xVelocity = -c1->xVelocity;
        }

        for (int j = i + 1; j < NumCircles; j++) {
            Circle* c2 = &circles[j];
            if (checkCollision(c1->xPos, c1->yPos, c1->radius, c2->xPos, c2->yPos, c2->radius)) {
                resolveCollision(&c1->xPos, &c1->yPos, &c1->xVelocity, &c1->yVelocity, c1->radius,
                                 &c2->xPos, &c2->yPos, &c2->xVelocity, &c2->yVelocity, c2->radius);
            }
        }
    }

}


