#include "Spaceship.h"
#include <cmath>
#include <stdexcept>
#include <vector>
#include "Global.h"

#define _USE_MATH_DEFINES

#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

// Definitions for the constants.
const float Spaceship::HPS[5] = {50, 3, 8, 15, 6};
const int Spaceship::DELAYS[4] = {50, 1500, 1000, 3000}; // In milliseconds.
const int Spaceship::SHOOTING_CHANCES[4] = {1, 300, 100, 500}; // The first element will never be used, as the flagship is human-controlled.
const float Spaceship::SPEEDS[5] = {0.4, 0.3, 0.45, 0.2, 0.6};
const float Spaceship::LENGTHS[5] = {32, 28, 44, 56, 24};
const float Spaceship::WIDTHS[5] = {32, 24, 40, 56, 40};

// Some fields are initialized straight in the constructor initializer, some depend on more things and are initialized below.
Spaceship::Spaceship(int s, float pos_x, float pos_y, int t, int l): side(s), direction(2), level(l), y(pos_y), temp(0), type(t), inCoolDown(false), lastShootTime(clock() - 100000), overheatTime(0)
{
    if (t == 0) // If it's a flagship, give it a temperature and hp according to its level.
        max_temp = max_hp = hp = HPS[0] + l * STAT_GAIN_PER_LEVEL;
    else if (t >= 1 && t < 5) // If it's another ship, just give its designated hp.
        max_hp = hp = HPS[t];
    else // Throw an error otherwise.
        throw domain_error("Ship creation failed. Ship types only range from 0-4");

    s == 1 ? x = pos_x : x = pos_x - LENGTHS[t]; // X represents the nose of the ship, and depends on which side it's on.

    // Checking boundaries.
    // Ships have to stay in the screen, on their side, and not cross the middle of the screen.
    if (side == 1)
    {
        if (x < 0)
            x = 0;
        else if (x > WIDTH / 2 - LENGTHS[type])
            x = WIDTH / 2 - LENGTHS[type];
    }
    else if (side == 2)
    {
        if (x < WIDTH / 2)
            x = WIDTH / 2;
        else if (x + LENGTHS[type] > WIDTH)
            x = WIDTH - LENGTHS[type];
    }
    if (y - WIDTHS[type] / 2 < 0)
        y = WIDTHS[type] / 2;
    else if (y + WIDTHS[type] / 2 > HEIGHT)
        y = HEIGHT - WIDTHS[type] / 2;
}

void Spaceship::Fire()
{
    if (type == 0)
        temp += 1; // Increase the temperature if it is the flagship.
    lastShootTime = clock(); // Update the shooting clock.
    direction = randn(9); // Randomize its direction.
}

bool Spaceship::CanFire() const
{
    if ((type == 0 && inCoolDown) || type == 4) // Kamikaze ship cannot fire, nor could a flagship in cooldown.
        return false;
    return clock() - lastShootTime > DELAYS[type]; // If it has passed its delay.
}

bool Spaceship::ReadyToFire() const
{
    if (type == 4) // Kamikaze can't fire.
        return false;
    else if (type == 0) // A flagship can fire when it can.
        return CanFire();
    else if (CanFire()) // An AI ship can fire when the chance allows.
        return randn(SHOOTING_CHANCES[type]) == 0;
    return false;
}

bool Spaceship::IsPointOnShip(float point_x, float point_y) const
{
    // Please see http://www.blackpawn.com/texts/pointinpoly/default.html
    // for explanation of point to triangle detection.

    // Declare vectors for triangles and rectangles.
    vector<Triangle> triangles;
    vector<Rect> rectangles;

    // The following generates triangles and rectangles the comprise each ship for collision detection.
    // They depend on which side the ship is on and what type of ship it is.
    if (side == 1)
    {
        switch (type)
        {
        case 0:
            triangles.push_back(Triangle(x, y + 8, x + 8, y + 16, x + 8, y + 8));
            triangles.push_back(Triangle(x, y - 8, x + 8, y - 16, x + 8, y - 8));
            triangles.push_back(Triangle(x + 16, y + 8, x + 16, y + 16, x + 32, y + 8));
            triangles.push_back(Triangle(x + 16, y - 8, x + 16, y - 16, x + 32, y - 8));
            rectangles.push_back(Rect(x + 8, y + 8, 8, 8));
            rectangles.push_back(Rect(x + 8, y - 16, 8, 8));
            rectangles.push_back(Rect(x, y - 8, 32, 16));
            break;
        case 1:
            triangles.push_back(Triangle(x + 4, y + 4, x + 4, y + 12, x + 12, y + 4));
            triangles.push_back(Triangle(x + 8, y + 4, x + 8, y + 12, x + 16, y + 4));
            triangles.push_back(Triangle(x + 4, y - 4, x + 4, y - 12, x + 12, y - 4));
            triangles.push_back(Triangle(x + 8, y - 4, x + 8, y - 12, x + 16, y - 4));
            triangles.push_back(Triangle(x + 20, y - 4, x + 20, y + 4, x + 28, y));
            rectangles.push_back(Rect(x - 4, y, 20, 8));
            break;
        case 2:
            triangles.push_back(Triangle(x + 8, y + 4, x + 8, y + 20, x + 20, y + 4));
            triangles.push_back(Triangle(x + 16, y + 4, x + 16, y + 20, x + 28, y + 4));
            triangles.push_back(Triangle(x + 8, y - 4, x + 8, y - 20, x + 20, y - 4));
            triangles.push_back(Triangle(x + 16, y - 4, x + 16, y - 20, x + 28, y - 4));
            triangles.push_back(Triangle(x + 36, y + 4, x + 36, y - 4, x + 44, y));
            rectangles.push_back(Rect(x, y - 4, 36, 8));
            break;
        case 3:
            triangles.push_back(Triangle(x + 48, y + 4, x + 48, y + 12, x + 64, y + 4));
            triangles.push_back(Triangle(x + 48, y - 4, x + 48, y - 12, x + 64, y - 4));
            rectangles.push_back(Rect(x, y + 20, 48, 8));
            rectangles.push_back(Rect(x, y - 28, 48, 8));
            rectangles.push_back(Rect(x + 8, y - 4, 48, 8));
            rectangles.push_back(Rect(x + 24, y - 20, 8, 40));
            break;
        case 4:
            triangles.push_back(Triangle(x, y + 20, x + 4, y + 4, x + 4, y + 2));
            triangles.push_back(Triangle(x + 12, y + 4, x + 12, y + 20, x + 16, y + 20));
            triangles.push_back(Triangle(x, y - 20, x + 4, y - 4, x + 4, y - 2));
            triangles.push_back(Triangle(x + 12, y - 4, x + 12, y - 20, x + 16, y - 20));
            triangles.push_back(Triangle(x + 16, y + 4, x + 16, y - 4, x + 24, y));
            rectangles.push_back(Rect(x + 4, y + 4, 8, 16));
            rectangles.push_back(Rect(x + 4, y - 20, 8, 16));
            rectangles.push_back(Rect(x, y - 4, 16, 8));
            break;
        }
    }
    else if (side == 2)
    {
        switch (type)
        {
        case 0:
            triangles.push_back(Triangle(x, y + 8, x + 16, y + 8, x + 16, y + 16));
            triangles.push_back(Triangle(x, y - 8, x + 16, y - 8, x + 16, y - 16));
            triangles.push_back(Triangle(x + 24, y + 8, x + 24, y + 16, x + 32, y + 8));
            triangles.push_back(Triangle(x + 24, y - 8, x + 24, y - 16, x + 32, y - 8));
            rectangles.push_back(Rect(x + 16, y + 8, 8, 8));
            rectangles.push_back(Rect(x + 16, y - 16, 8, 8));
            rectangles.push_back(Rect(x, y - 8, 32, 16));
            break;
        case 1:
            triangles.push_back(Triangle(x + 12, y + 4, x + 20, y + 4, x + 20, y + 12));
            triangles.push_back(Triangle(x + 16, y + 4, x + 24, y + 4, x + 24, y + 12));
            triangles.push_back(Triangle(x + 12, y - 4, x + 20, y - 4, x + 20, y - 12));
            triangles.push_back(Triangle(x + 16, y - 4, x + 24, y - 4, x + 24, y - 12));
            triangles.push_back(Triangle(x, y, x + 8, y + 4, x + 8, y - 4));
            rectangles.push_back(Rect(x + 8, y - 4, 20, 8));
            break;
        case 2:
            triangles.push_back(Triangle(x + 16, y + 4, x + 28, y + 4, x + 28, y + 20));
            triangles.push_back(Triangle(x + 24, y + 4, x + 36, y + 4, x + 36, y + 20));
            triangles.push_back(Triangle(x + 16, y - 4, x + 28, y - 4, x + 28, y - 20));
            triangles.push_back(Triangle(x + 24, y - 4, x + 36, y - 4, x + 36, y - 20));
            triangles.push_back(Triangle(x, y, x + 8, y + 4, x + 8, y - 4));
            rectangles.push_back(Rect(x + 8, y - 4, 36, 8));
            break;
        case 3:
            triangles.push_back(Triangle(x, y + 4, x + 16, y + 4, x + 16, y + 12));
            triangles.push_back(Triangle(x, y - 4, x + 16, y - 4, x + 16, y - 12));
            rectangles.push_back(Rect(x + 16, y + 20, 48, 8));
            rectangles.push_back(Rect(x + 16, y - 28, 48, 8));
            rectangles.push_back(Rect(x + 32, y - 20, 8, 40));
            rectangles.push_back(Rect(x, y - 4, 48, 8));
            break;
        case 4:
            triangles.push_back(Triangle(x + 8, y + 20, x + 12, y + 4, x + 12, y + 20));
            triangles.push_back(Triangle(x + 20, y + 4, x + 20, y + 20, x + 24, y + 20));
            triangles.push_back(Triangle(x + 8, y - 20, x + 12, y - 4, x + 12, y - 20));
            triangles.push_back(Triangle(x + 20, y - 4, x + 20, y - 20, x + 24, y - 20));
            triangles.push_back(Triangle(x, y, x + 8, y + 4, x + 8, y - 4));
            rectangles.push_back(Rect(x + 12, y + 4, 8, 16));
            rectangles.push_back(Rect(x + 12, y - 20, 8, 16));
            rectangles.push_back(Rect(x + 8, y - 4, 16, 8));
            break;
        }
    }

    // Check if any of the rectangles intersect with the point.
    for (vector<Rect>::const_iterator it = rectangles.begin(); it != rectangles.end(); ++it)
    {
        if (it->IsPointInRectangle(point_x, point_y))
            return true;
    }
    // Check if any of the triangles. intersect with the point.
    for (vector<Triangle>::const_iterator it = triangles.begin(); it != triangles.end(); ++it)
    {
        if (it->IsPointInTriangle(point_x, point_y))
            return true;
    }
    return false;
}

void Spaceship::Periodic()
{
    // For a flagship, there's cooldown to manage.
    if (type == 0)
    {
        // If in cooldown, check if cooldown time has finished.
        if (inCoolDown && clock() - overheatTime >= COOLDOWN_TIME * (1 + (level * STAT_GAIN_PER_LEVEL) / float(HPS[0])))
        {
            inCoolDown = false; // If it is, it's no longer in cooldown.
            temp = 0; // And temperature restores to 0.
        }
        // Check if temperature reached maximum.
        else if (!inCoolDown && temp >= max_temp)
        {
            inCoolDown = true; // Then it is in cooldown.
            overheatTime = clock(); // Set the overheating time to now.
        }
        // Periodic decrease in temperature, making sure that it doesn't go below 0.
        else if (!inCoolDown && temp > 0)
            temp >= COOLDOWN_SPEED ? temp -= COOLDOWN_SPEED : temp = 0;
    }
    // For a kamikaze ship, just move... it's suicidal anyways...
    else if (type == 4)
    {
        if (side == 1)
            x += SPEEDS[4];
        else
            x -= SPEEDS[4];
    }
    // Otherwise move according to the direction of the A.I.
    else
    {
        // The number stored in direction represents a unique direction.
        // It starts from right & up with 1, and rotates clockwise.
        switch (direction)
        {
        case 0: // 0 means stop.
            break;
        case 1: // 1 means up & right.
            MoveUp();
        case 2: // 2 means just right.
            MoveRight();
            break;
        case 3: // 3 means right and down.
            MoveRight();
        case 4: // 4 means just down.
            MoveDown();
            break;
        case 5: // 5 means down and left.
            MoveDown();
        case 6: // 6 means just left.
            MoveLeft();
            break;
        case 7: // 7 means left and up.
            MoveLeft();
        case 8: // 8 means just up.
            MoveUp();
            break;
        default: // Only cardinal & secondary directions are included.
            throw domain_error("Invalid direction; directions range from 0-8.");
        }
    }
}

// Move left so that it doesn't go out of its boundaries, stated in a comment in the constructor.
void Spaceship::MoveLeft()
{
    if (side == 1 && x > 0 && type != 4)
        x -= SPEEDS[type];
    else if ((side == 2 && x > WIDTH / 2) || type == 4)
        x -= SPEEDS[type];
}

// Move right so that it doesn't go out of its boundaries, stated in a comment in the constructor.
void Spaceship::MoveRight()
{
    if ((side == 1 && x + LENGTHS[type] < WIDTH / 2) || type == 4)
        x += SPEEDS[type];
    else if (side == 2 && x + LENGTHS[type] < WIDTH && type != 4)
        x += SPEEDS[type];
}

// Move up so that it doesn't go out of its boundaries, stated in a comment in the constructor.
void Spaceship::MoveUp()
{
    if (y + WIDTHS[type] / 2 < HEIGHT)
        y += SPEEDS[type] * Y_MOVE_AS_PERCENTAGE_OF_MAX_SPEED;
}

// Move down so that it doesn't go out of its boundaries, stated in a comment in the constructor.
void Spaceship::MoveDown()
{
    if (y - WIDTHS[type] / 2 > 0)
        y -= SPEEDS[type] * Y_MOVE_AS_PERCENTAGE_OF_MAX_SPEED;
}

void Spaceship::DrawHP() const
{
    // Draw a rectangle for the hp.
    glBegin(GL_QUADS);
        glColor4f(0, 1, 0, 0.5);
        glVertex2f(x, y + 19);
        glVertex2f(x + 32 * hp / max_hp, y + 19); // The length depends on the % of hp.
        glVertex2f(x + 32 * hp / max_hp, y + 22);
        glVertex2f(x, y + 22);
    glEnd();
}

void Spaceship::DrawCooldown() const
{
    // Draw a rectangle for the temperature.
    glBegin(GL_QUADS);
        glColor4f(1, 0, 0, 0.5);
        glVertex2f(x, y - 19);
        glVertex2f(x + 32 * temp / max_temp, y - 19); // The length depends on the % of temperature.
        glVertex2f(x + 32 * temp / max_temp, y - 22);
        glVertex2f(x, y - 22);
    glEnd();
}

void Spaceship::Draw() const
{
    // The following draws the ships.
    // They depend on which side the ship is on and what type of ship it is.
    if (side == 1)
    {
        switch (type)
        {
        case 0:
            // The body
            glBegin(GL_POLYGON);
                glColor3f(0.6, 0.6, 0.6);
                glVertex2f(x, y + 8);
                glVertex2f(x + 8, y + 16);
                glVertex2f(x + 16, y + 16);
                glVertex2f(x + 32, y + 8);
                glVertex2f(x + 32, y + 4);
                glVertex2f(x + 24, y);
                glVertex2f(x + 32, y - 4);
                glVertex2f(x + 32, y - 8);
                glVertex2f(x + 16, y - 16);
                glVertex2f(x + 8, y - 16);
                glVertex2f(x, y - 8);
            glEnd();
            // The Top
            glBegin(GL_POLYGON);
                glColor3f(0, 0, 0.8);
                for (float a = 0; a < 2 * M_PI; a += M_PI / 50)
                    glVertex2f(8 * cos(a) + x + 16, 8 * sin(a) + y); // Uses the unit circle.
            glEnd();
            break;

        case 1:
            // The body
            glBegin(GL_QUADS);
                glColor3f(0.8, 0.8, 0.8);
                glVertex2f(x, y + 4);
                glVertex2f(x + 20, y + 4);
                glVertex2f(x + 20, y - 4);
                glVertex2f(x, y - 4);
            glEnd();
            glBegin(GL_TRIANGLES);
                // The nose
                glVertex2f(x + 20, y + 4);
                glVertex2f(x + 28, y);
                glVertex2f(x + 20, y - 4);
                // Top back.
                glColor3f(0.3, 0.3, 1);
                glVertex2f(x + 8, y + 12);
                glVertex2f(x + 16, y + 4);
                glVertex2f(x + 8, y + 4);
                // Bottom back.
                glVertex2f(x + 8, y - 4);
                glVertex2f(x + 16, y - 4);
                glVertex2f(x + 8, y - 12);
                // Top front.
                glColor3f(0.5, 0.5, 0.5);
                glVertex2f(x + 4, y + 12);
                glVertex2f(x + 12, y + 4);
                glVertex2f(x + 4, y + 4);
                // Bottom front
                glVertex2f(x + 4, y - 4);
                glVertex2f(x + 12, y - 4);
                glVertex2f(x + 4, y - 12);
            glEnd();
            break;

        case 2:
            // Body
            glBegin(GL_QUADS);
                glColor3f(0.9, 0.9, 0.9);
                glVertex2f(x, y + 4);
                glVertex2f(x + 36, y + 4);
                glVertex2f(x + 36, y - 4);
                glVertex2f(x, y - 4);
            glEnd();
            glBegin(GL_TRIANGLES);
                // Nose
                glVertex2f(x + 36, y + 4);
                glVertex2f(x + 44, y);
                glVertex2f(x + 36, y - 4);
                // Top back
                glColor3f(0.2, 0.2, 1);
                glVertex2f(x + 16, y + 4);
                glVertex2f(x + 16, y + 20);
                glVertex2f(x + 28, y + 4);
                // Bottom back.
                glVertex2f(x + 16, y - 4);
                glVertex2f(x + 16, y - 20);
                glVertex2f(x + 28, y - 4);
                // Top front.
                glColor3f(0.6, 0.6, 0.6);
                glVertex2f(x + 8, y + 4);
                glVertex2f(x + 8, y + 20);
                glVertex2f(x + 20, y + 4);
                // Bottom front.
                glVertex2f(x + 8, y - 4);
                glVertex2f(x + 8, y - 20);
                glVertex2f(x + 20, y - 4);
            glEnd();
            break;

        case 3:
            glBegin(GL_QUADS);
                // Sides
                glColor3f(0.8, 0.8, 0);
                glVertex2f(x, y + 28);
                glVertex2f(x + 48, y + 28);
                glVertex2f(x + 48, y + 20);
                glVertex2f(x, y + 20);
                glVertex2f(x + 24, y + 20);
                glVertex2f(x + 32, y + 20);
                glVertex2f(x + 32, y + 4);
                glVertex2f(x + 24, y + 4);
                glVertex2f(x, y - 28);
                glVertex2f(x + 48, y - 28);
                glVertex2f(x + 48, y - 20);
                glVertex2f(x, y - 20);
                glVertex2f(x + 24, y - 20);
                glVertex2f(x + 32, y - 20);
                glVertex2f(x + 32, y - 4);
                glVertex2f(x + 24, y - 4);
                // Body
                glColor3f(0.3, 0.3, 1);
                glVertex2f(x + 16, y + 4);
                glVertex2f(x + 48, y + 4);
                glVertex2f(x + 48, y - 4);
                glVertex2f(x + 16, y - 4);
                // Head
                glColor3f(0.6, 0.6, 0.6);
                glVertex2f(x + 48, y + 12);
                glVertex2f(x + 64, y + 4);
                glVertex2f(x + 64, y - 4);
                glVertex2f(x + 48, y - 12);
            glEnd();
            break;

        case 4:
            // Body
            glBegin(GL_POLYGON);
                glColor3f(0, 0.7, 0.7);
                glVertex2f(x, y + 4);
                glVertex2f(x + 16, y + 4);
                glVertex2f(x + 24, y);
                glVertex2f(x + 16, y - 4);
                glVertex2f(x, y - 4);
            glEnd();
            // Wings
            glBegin(GL_QUADS);
                glColor3f(0.2, 0.2, 1);
                glVertex2f(x + 4, y + 4);
                glVertex2f(x, y + 20);
                glVertex2f(x + 16, y + 20);
                glVertex2f(x + 12, y + 4);
                glVertex2f(x + 4, y - 4);
                glVertex2f(x, y - 20);
                glVertex2f(x + 16, y - 20);
                glVertex2f(x + 12, y - 4);
            glEnd();
            break;

        default: // Throw an error if the ship is not defined.
            throw domain_error("Cannot create ship; ship type does not exist.");
            break;
        }

        if (type == 0)
        {
            DrawHP();
            DrawCooldown();
        }
    }
    else if (side == 2)
    {
        switch (type)
        {
        case 0:
            // Body.
            glBegin(GL_POLYGON);
                glColor3f(0.5, 0.5, 0.5);
                glVertex2f(x + 8, y);
                glVertex2f(x, y + 4);
                glVertex2f(x, y + 8);
                glVertex2f(x + 16, y + 16);
                glVertex2f(x + 24, y + 16);
                glVertex2f(x + 32, y + 8);
                glVertex2f(x + 32, y - 8);
                glVertex2f(x + 23, y - 16);
                glVertex2f(x + 16, y - 16);
                glVertex2f(x, y - 8);
                glVertex2f(x, y - 4);
            glEnd();
            // Top.
            glBegin(GL_POLYGON);
                glColor3f(1, 0, 0);
                for (float a = 0; a < 2 * M_PI; a += M_PI / 50)
                    glVertex2f(8 * cos(a) + x + 16, 8 * sin(a) + y);
            glEnd();
            break;

        case 1:
            // Body.
            glBegin(GL_QUADS);
                glColor3f(0.8, 0.8, 0.8);
                glVertex2f(x + 8, y + 4);
                glVertex2f(x + 28, y + 4);
                glVertex2f(x + 28, y - 4);
                glVertex2f(x + 8, y - 4);
            glEnd();
            glBegin(GL_TRIANGLES);
                // Nose.
                glVertex2f(x, y);
                glVertex2f(x + 8, y + 4);
                glVertex2f(x + 8, y - 4);
                // Top front.
                glColor3f(1, 0.3, 0.3);
                glVertex2f(x + 12, y + 4);
                glVertex2f(x + 20, y + 12);
                glVertex2f(x + 20, y + 4);
                // Bottom front.
                glVertex2f(x + 12, y - 4);
                glVertex2f(x + 20, y - 12);
                glVertex2f(x + 20, y - 4);
                // Top back.
                glColor3f(0.5, 0.5, 0.5);
                glVertex2f(x + 16, y + 4);
                glVertex2f(x + 24, y + 12);
                glVertex2f(x + 24, y + 4);
                // Bottom back.
                glVertex2f(x + 16, y - 4);
                glVertex2f(x + 24, y - 12);
                glVertex2f(x + 24, y - 4);
            glEnd();
            break;

        case 2:
            // Body.
            glBegin(GL_QUADS);
                glColor3f(0.9, 0.9, 0.9);
                glVertex2f(x + 8, y + 4);
                glVertex2f(x + 44, y + 4);
                glVertex2f(x + 44, y - 4);
                glVertex2f(x + 8, y - 4);
            glEnd();
            glBegin(GL_TRIANGLES);
                // Nose.
                glVertex2f(x, y);
                glVertex2f(x + 8, y + 4);
                glVertex2f(x + 8, y - 4);
                // Top front.
                glColor3f(1, 0.2, 0.2);
                glVertex2f(x + 16, y + 4);
                glVertex2f(x + 28, y + 20);
                glVertex2f(x + 28, y + 4);
                // Bottom front.
                glVertex2f(x + 16, y - 4);
                glVertex2f(x + 28, y - 20);
                glVertex2f(x + 28, y - 4);
                // Top back.
                glColor3f(0.6, 0.6, 0.6);
                glVertex2f(x + 24, y + 4);
                glVertex2f(x + 36, y + 20);
                glVertex2f(x + 36, y + 4);
                // Bottom back.
                glVertex2f(x + 24, y - 4);
                glVertex2f(x + 36, y - 20);
                glVertex2f(x + 36, y - 4);
            glEnd();
            break;

        case 3:
            glBegin(GL_QUADS);
                // Sides
                glColor3f(0.8, 0.8, 0);
                glVertex2f(x + 16, y + 28);
                glVertex2f(x + 64, y + 28);
                glVertex2f(x + 64, y + 20);
                glVertex2f(x + 16, y + 20);
                glVertex2f(x + 32, y + 20);
                glVertex2f(x + 40, y + 20);
                glVertex2f(x + 40, y + 4);
                glVertex2f(x + 32, y + 4);
                glVertex2f(x + 16, y - 28);
                glVertex2f(x + 64, y - 28);
                glVertex2f(x + 64, y - 20);
                glVertex2f(x + 16, y - 20);
                glVertex2f(x + 32, y - 20);
                glVertex2f(x + 40, y - 20);
                glVertex2f(x + 40, y - 4);
                glVertex2f(x + 32, y - 4);
                // Body
                glColor3f(1, 0.3, 0.3);
                glVertex2f(x + 16, y + 4);
                glVertex2f(x + 48, y + 4);
                glVertex2f(x + 48, y - 4);
                glVertex2f(x + 16, y - 4);
                // Head
                glColor3f(0.6, 0.6, 0.6);
                glVertex2f(x, y + 4);
                glVertex2f(x + 16, y + 12);
                glVertex2f(x + 16, y - 12);
                glVertex2f(x, y - 4);
            glEnd();
            break;

        case 4:
            // Body.
            glBegin(GL_POLYGON);
                glColor3f(0, 0.7, 0.7);
                glVertex2f(x, y);
                glVertex2f(x + 8, y + 4);
                glVertex2f(x + 24, y + 4);
                glVertex2f(x + 24, y - 4);
                glVertex2f(x + 8, y - 4);
            glEnd();
            // Wings.
            glBegin(GL_QUADS);
                glColor3f(1, 0.2, 0.2);
                glVertex2f(x + 12, y + 4);
                glVertex2f(x + 8, y + 20);
                glVertex2f(x + 24, y + 20);
                glVertex2f(x + 20, y + 4);
                glVertex2f(x + 12, y - 4);
                glVertex2f(x + 8, y - 20);
                glVertex2f(x + 24, y - 20);
                glVertex2f(x + 20, y - 4);
            glEnd();
            break;

        default: // Throw an error if the ship is not defined.
            throw domain_error("Cannot create ship; ship type does not exist.");
            break;
        }

        // If it's a flagship, draw the hp and cooldown bars.
        if (type == 0)
        {
            DrawHP();
            DrawCooldown();
        }
    }
}
