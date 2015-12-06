#include "Global.h"
#include <cstdlib>
#include <sstream>
#include <cmath>

#define _USE_MATH_DEFINES // for using M_PI.

#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

int randn(int limit)
{
    int bucket_size = RAND_MAX / limit; // This gives an equal chance for any number to be generated.

    int r; // Declares a returning integer.
    do r = std::rand() / bucket_size; // Get a random number.
    while (r >= limit); // Until it is within the limit.

    return r;
}

void DrawString(string s, float x, float y)
{
    glRasterPos2f(x, y); // Set the position.
    // Go through the characters of the string and print them to the screen.
    for (string::size_type i = 0; i != s.size(); ++i)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
}

string IntToString(int n)
{
    stringstream ss; // Create a stringstream.
    ss << n; // Add number to the stream.
    return ss.str(); // Return a string with the contents in the stream.
}

void DrawStar(float l, float x, float y)
{
    // For explanation please see the RAT Reflection word document.
    const float la = l / (2 * (sin(M_PI / 10) + 1));
    const float li = (l * sin(M_PI / 10)) / (1 + sin(M_PI / 10));

    const float x2 = x + la * sin(3 * M_PI / 20), y2 = y + la * cos(3 * M_PI / 20);
    const float x3 = x2 - la * cos (M_PI / 5), y3 = y2 + la * sin(M_PI / 5);

    const float sx[] = {x, x2, x3, x3 + la, x3 + l / 2, x3 + la + li, x3 + l, x + (la + li) * cos(3 * M_PI / 20), x + 2 * la * cos(3 * M_PI / 20), x3 + l / 2};
    const float sy[] = {y, y2, y3, y3, y + l * cos(3 * M_PI / 20), y3, y3, y2, y, y + la * sin(3 * M_PI / 20)};

    glColor3ub(15, 77, 146);
    // Draw the triagles of the stars.
    glBegin(GL_TRIANGLES);
        glVertex2f(sx[9], sy[9]);
        for (size_t i = 0; i != 2; ++i)
            glVertex2f(sx[i], sy[i]);
        for (size_t i = 1; i != 4; ++i)
            glVertex2f(sx[i], sy[i]);
        for (size_t i = 3; i != 6; ++i)
            glVertex2f(sx[i], sy[i]);
        for (size_t i = 5; i != 8; ++i)
            glVertex2f(sx[i], sy[i]);
        for (size_t i = 7; i != 10; ++i)
            glVertex2f(sx[i], sy[i]);
    glEnd();
    // Draw the pentagon of the centre of the star.
    glBegin(GL_POLYGON);
        for (size_t i = 1; i < 10; i += 2)
            glVertex2f(sx[i], sy[i]);
    glEnd();
}

Vec Vec::CrossProduct(const Vec& other) const // Cross product.
{
    return Vec(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

bool Rect::IsPointInRectangle(float point_x, float point_y) const // Basic rectangle collision detection.
{
    return point_x >= x && point_y >= y && point_x <= x + length && point_y <= y + width;
}

bool Triangle::IsPointInTriangle(float x, float y) const
{
    // For explanation, please see http://www.blackpawn.com/texts/pointinpoly/default.html
    Vec side1(x1, y1, x2, y2), side2(x2, y2, x3, y3), side3(x3, y3, x1, y1), check1(x1, y1, x, y);
    Vec cross1 = side1.CrossProduct(check1), cross2 = side1.CrossProduct(side3.Opposite());
    if (cross1.DotProduct(cross2) < 0)
        return false;

    Vec check2(x2, y2, x, y);
    cross1 = side2.CrossProduct(check2), cross2 = side2.CrossProduct(side1.Opposite());
    if (cross1.DotProduct(cross2) < 0)
        return false;

    Vec check3(x3, y3, x, y);
    cross1 = side3.CrossProduct(check3), cross2 = side3.CrossProduct(side2.Opposite());
    if (cross1.DotProduct(cross2) < 0)
        return false;
    return true;
}
