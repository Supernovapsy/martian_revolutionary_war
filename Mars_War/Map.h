#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <vector>
#include "Base.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "Global.h"

class Map
{
public:
    Map(); // Constructor.
    // Periodic functions.
    void DrawBases();
    int WarPeriodic();
    void PeacePeriodic();

    // Selection functions for upgrades.
    void TerranSelectionUp();
    void TerranSelectionDown();
    void MartianSelectionUp();
    void MartianSelectionDown();
    void TerranSelectionConfirm() { terranBase.MakePurchase(t); }
    void TerranSelectionUndo() { terranBase.UndoPurchase(t); }
    void MartianSelectionConfirm() { martianBase.MakePurchase(m); }
    void MartianSelectionUndo() { martianBase.UndoPurchase(m); }

    // Initializers for peace and nextLevel (war).
    void Peace();
    void NextLevel();

    // Controlling the flagships.
    void ControlTerranFlagship(char);
    void ControlMartianFlagship(char);

private:
    // Constants: self-explanatory.
    static const char* const DESCRIPTIONS[5];
    static const float SEPARATION = HEIGHT / 6; // Separation between model ships.

    Base terranBase, martianBase; // Terran and martian bases.
    std::vector<Spaceship> modelShips; // Vector to store the model ships.
    Spaceship terranSelectionShip, martianSelectionShip; // The selection ships for the terran and martian sides.
    int t, m; // Integers representing the position of the selection ship.
    int level; // Current level.

    // If the players get past round 1, they gain 1 resource; 2, they get 4; 3, they get 9... etc.
    int GetBonusResourcesByLevel(int l) const { return l * l; }
};

#endif // MAP_H_INCLUDED
