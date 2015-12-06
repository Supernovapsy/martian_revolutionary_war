#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include <list>
#include <cmath>
#include "Spaceship.h"
#include "Bullet.h"
#include "Global.h"

// A structure for the shields.
struct Shield
{
    bool down; // Whether or not it's been destroyed.
    float distance, hp; // Its distance from the side of the map and its hp.
};

bool EraseShipPred(const Spaceship&); // A predicate function for an STL template function.

class Base
{
public:
    Base(int, float, float); // Constructor.

    // Drawing functions.
    void DrawBase() const;
    void DrawShips() const;
    void DrawBullets() const;

    // Accessor Functions.
    int GetResources() const { return resources; }
    int GetShipsAvailable(int) const; // Depending on which ship the player's looking at.
    int GetUpgrade(int) const; // Depending on which upgrade the player's looking at.
    int GetUnfinalizedUpgrade(int) const; // Depending on which upgrade the player's looking at.

    // More accessors.
    bool ShieldDown(); /// Make sure this function is called in conjuction WITH periodic()
    bool IsAlive() const { return hp > 0; }

    // Even more accessors that are self-explanatory.
    void EarnResources(int r) { resources += r; }
    void MakePurchase(int); // Depending on which upgrade the player's looking at.
    void UndoPurchase(int); // Depending on which upgrade the player's looking at.
    void FinalizePurchase();
    void GiveLoot(Base&);
    void GatherInterest() { resources *= INTEREST_RATE; }

    // Accessors...
    std::list<Spaceship>& GetShips() { return ships; }
    std::list<Bullet>& GetBullets() { return bullets; }

    void Peace(Base&); // The peace initializer function (called at beginning of peace).
    void NextLevel(); // The nextlevel initializer function.
    void Periodic(std::list<Spaceship>&, std::list<Bullet>&); // The periodic function.

    // Flagship controls.
    void FlagshipMoveUp() { if (flagshipAlive) flagship_p->MoveUp(); }
    void FlagshipMoveDown() { if (flagshipAlive) flagship_p->MoveDown(); }
    void FlagshipMoveLeft() { if (flagshipAlive) flagship_p->MoveLeft(); }
    void FlagshipMoveRight() { if (flagshipAlive) flagship_p->MoveRight(); }
    void FlagshipFire();

private:
    // Constants: self-explanatory.
    static const int BASE_SPAWNING_CHANCES[5], UPGRADE_LIMIT = 20;
    static const int INITIAL_RESOURCES = 1000, MIN_LOOT_PERCENTAGE = 15, MAX_LOOT_PERCENTAGE = 30, UPGRADE_COSTS[5], SHIP_KILL_BONUSES[5];
    static const float SHIELD_1_HP = 750, SHIELD_2_HP = 1000, SHIELD_1_DISTANCE = 150, SHIELD_2_DISTANCE = 120, SHIELD_WIDTH = 20;
    static const float INTEREST_RATE = 1.1, BASE_HP = 1500, BASE_DISTANCE = 50;

    void SpawnFlagship(); // Spawn the flagship.

    int side; // Which side the base is on.
    float x, y, radius, hp; // The base's position, radius for the planet and shield, and hp.
    // The first three are self-explanatory, the last two are arrays which store ships in stock and unfinalized upgrade purchases.
    int resources, factoryLevel, flagshipLevel, shipsAvailable[5], unfinalizedPurchases[5];
    Shield shield1, shield2; // Shields
    std::list<Spaceship> ships; // The ships.
    std::list<Bullet> bullets; // The bullets.
    std::list<Spaceship>::iterator flagship_p; // A pointer that points to the flagship.
    bool flagshipAlive, doNotSpawnFlagship; // Self-explanatory booleans for the flagship.
    int resourcesForOtherBase; // The amount of bonus resources for the other base at the end of the level for ships destroyed.

    // This function gets the base spawning chance for ships and gives its actual spawning chance depending on upgrade levels (It radically (sqrt), decreases as the level of upgrade increases).
    int GetChanceFromLevelThroughBaseChance(int bc, int l) const { return -bc / 5 * std::sqrt(l) + bc; }
};

#endif // BASE_H_INCLUDED
