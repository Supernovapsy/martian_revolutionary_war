#include "Bullet.h"
#include <stdexcept>

#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

// Definitions for the constants.
const float Bullet::LENGTHS[4] = {4, 2, 4, 8};
const float Bullet::WIDTHS[4] = {0, 0, 0, 0};
const float Bullet::SPEEDS[4] = {1, 0.75, 0.75, 0.5};
const float Bullet::POWERS[4] = {0.3, 2, 4.5, 10};

// Some fields are initialized straight in the constructor initializer, some depend on more things and are initialized below.
Bullet::Bullet(int s, float pos_x, float pos_y, int t, int l): side(s), type(t)
{
    if (t < 0 || t > 3) // Incase a wrong type is inputted, throw an error.
        throw domain_error("Bullet does not exist; bullet types only range from 0-3.");

    // Initialize class fields.
    length = LENGTHS[t];
    width = WIDTHS[t];
    speed = SPEEDS[t];
    power = POWERS[t];
    // If it's a flagship's bullet, get its power depending on the ship's level.
    if (t == 0)
        power += l * POWER_GAIN_PER_LEVEL;

    // Since the x positions of the bullet is always the left of the bullet, when it spawns on the martian side (2), it has to move left by its length.
    side == 1 ? x = pos_x
              : x = pos_x - length;
    y = pos_y - width / 2; // The y position is the middle of the bullet.
}

void Bullet::Draw() const
{
    // If the width is 0, draw a line.
    if (width == 0)
    {
        glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(x + length, y);
        glEnd();
    }
    // If the width is not zero, draw a rectangle.
    else
    {
        glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x, y + width);
            glVertex2f(x + length, y + width);
            glVertex2f(x + length, y);
        glEnd();
    }
}
