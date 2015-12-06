#ifndef SPACESHIP_H_INCLUDED
#define SPACESHIP_H_INCLUDED

#include <ctime>

class Spaceship
{
public:
    static const float KAMIKAZE_DAMAGE = 100;

    Spaceship(int, float, float, int, int = 0); // Constructor.
    // Drawing functions.
    void Draw() const;
    void DrawHP() const;
    void DrawCooldown() const;

    // Accessor functions.
    float GetX() const { return side == 1 ? x + LENGTHS[type] : x; } // This function returns the x-value of the tip of the ship, so it is different depending on which side the ship is on.
    float GetTailX() const { return side == 1 ? x : x + LENGTHS[type]; } // This returns the x-value of the tail of the ship.
    float GetY() const { return y; }
    float GetHP() const { return hp; }
    bool IsAlive() const { return hp > 0; }
    int GetType() const { return type; }
    int GetSide() const { return side; }
    int GetLevel() const { return level; }
    bool ReadyToFire() const;

    void Fire(); // Firing function.
    void HitBase() { if (type == 4) hp = 0; } // If the ship hits the other's base.

    bool IsPointOnShip(float, float) const; // Check if a point is on the ship.
    void DamagedBy(float d) { hp -= d; } // Lower hp due to damage.

    // Move functions.
    void MoveY(float y_move) { y += y_move; }
    void MoveLeft();
    void MoveRight();
    void MoveUp();
    void MoveDown();

    void Periodic(); // Periodic function.

private:
    // Constants: self-explanatory.
    static const int COOLDOWN_TIME = 7500, SHOOTING_CHANCES[4], DELAYS[4];
    static const float HPS[5], STAT_GAIN_PER_LEVEL = 10;
    static const float SPEEDS[5], LENGTHS[5], WIDTHS[5], Y_MOVE_AS_PERCENTAGE_OF_MAX_SPEED = 0.5, COOLDOWN_SPEED = 0.01;

    bool CanFire() const; // If the ship can fire.

    int side, direction, level; // Which side, direction the ship AI is going, the level of the ship (only applies for flagship).
    float x, y, hp, max_hp, temp, max_temp; // x, y values of the ship, hp & max hp, temperature (for overheating) & max_temp.
    int type; // Type of ship.
    bool inCoolDown; // Whether or not it's in cooldown.
    clock_t lastShootTime, overheatTime; // The last time it shot, and when it overheated.
};

#endif // SPACESHIP_H_INCLUDED
