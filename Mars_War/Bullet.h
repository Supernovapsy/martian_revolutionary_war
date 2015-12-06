#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "Global.h"

class Bullet
{
public:
    // Constants: self-explanatory.
    static const float LENGTHS[4], WIDTHS[4], SPEEDS[4], POWERS[4];

    Bullet(int, float, float, int, int = 0); // Constructor.
    void Draw() const; // Draws the bullet.

    // Accessor functions.
    int GetType() const { return type; }
    int GetSide() const { return side; }
    float GetLength() const { return length; }
    float GetWidth() const { return width; }
    float GetPower() const { return power; }
    float GetBasePower() const { return type == 3 ? 50 : power; } // The power of the bullet if it hits a shield or planet.
    float GetSpeed() const { return -(side * 2 - 3) * speed; } // The speed is the same, but directions are different, so it depends on the side.

    // More accessor functions.
    float GetXLeft() const { return x; }
    float GetXRight() const { return x + length; }
    float GetY() const { return y; }

    // Move (periodic).
    void Move() { side == 1 ? x += SPEEDS[type] : x -= SPEEDS[type]; }

    bool IsLaser() const { return width == 0; } // If it is a thin line.
    bool OutOfBounds() const { return x + LENGTHS[type] < 0 || x > WIDTH; } // If it is out of the map.

private:
    // Constants: self-explanatory.
    static const float POWER_GAIN_PER_LEVEL = 0.1, BASE_POWER = 50;

    // Self-explanatory.
    float x, y, length, width, speed, power;
    int side, type;
};

#endif // BULLET_H_INCLUDED
