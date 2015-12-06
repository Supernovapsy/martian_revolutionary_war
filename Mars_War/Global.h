// This file supplies functions and structures to all files to use.
#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <string>

// Width and height of the game screen.
const int WIDTH = 1000, HEIGHT = 670;

int randn(int); // Random number function (0 - int).
void DrawString(std::string, float, float); // Draw a string onto the screen at (x, y).
std::string IntToString(int); // Turn an integer into a string.
void DrawStar(float, float, float); // Draw a star of a certain size onto the screen at (x, y).

// A Vector structure (the math vector).
struct Vec
{
    // Construct a vector through giving its positional vector values.
    Vec(float a, float b, float c): x(a), y(b), z(c) { }
    // Construct a vector through giving two points.
    Vec(float x1, float y1, float x2, float y2): x(x2 - x1), y(y2 - y1), z(0) { }

    // Dot product with another vector.
    float DotProduct(const Vec& other) { return x * other.x + y * other.y + z * other.z; }
    // Cross product with another vector.
    Vec CrossProduct(const Vec&) const;
    // Return the opposite vector.
    Vec Opposite() const { return Vec(-x, -y, -z); }

    float x, y, z; // The fields of the vector.
};

struct Rect
{
    // Construct a rectangle through its bottom left corner and its length and width.
    Rect(float x_pos, float y_pos, float l, float w): x(x_pos), y(y_pos), length(l), width(w) { }

    bool IsPointInRectangle(float, float) const; // Returns whether or not a point is in the rectangle.

    int x, y, length, width; // Fields.
};

struct Triangle
{
    // Construct a triangle by giving three points that define the triangle.
    Triangle(float x1p, float y1p, float x2p, float y2p, float x3p, float y3p): x1(x1p), y1(y1p), x2(x2p), y2(y2p), x3(x3p), y3(y3p) { }

    bool IsPointInTriangle(float, float) const; // Returns whether or not a point is in the triangle.

    float x1, y1, x2, y2, x3, y3; // Fields.
};

#endif // GLOBAL_H_INCLUDED
