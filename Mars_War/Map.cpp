#include "Map.h"
#include <cstddef>

#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

const char* const Map::DESCRIPTIONS[5] = {"Upgrade flagship", "Upgrade ship factory", "Construct fighter", "Construct bomber", "Construct kamikaze"};

// Some fields are initialized straight in the constructor initializer, some depend on more things and are initialized below.
Map::Map(): terranBase(1, 0, HEIGHT / 2), martianBase(2, WIDTH, HEIGHT / 2), terranSelectionShip(1, 50, SEPARATION, 2), martianSelectionShip(2, WIDTH - 80, SEPARATION, 2), t(0), m(0), level(0)
{
    // Terran ships with corresponding separations.
    modelShips.push_back(Spaceship(1, 100, SEPARATION, 0));
    modelShips.push_back(Spaceship(1, 100, 2 * SEPARATION, 1));
    modelShips.push_back(Spaceship(1, 100, 3 * SEPARATION, 2));
    modelShips.push_back(Spaceship(1, 100, 4 * SEPARATION, 3));
    modelShips.push_back(Spaceship(1, 100, 5 * SEPARATION, 4));
    // Martian ships.
    modelShips.push_back(Spaceship(2, WIDTH - 150, SEPARATION, 0));
    modelShips.push_back(Spaceship(2, WIDTH - 150, 2 * SEPARATION, 1));
    modelShips.push_back(Spaceship(2, WIDTH - 150, 3 * SEPARATION, 2));
    modelShips.push_back(Spaceship(2, WIDTH - 150, 4 * SEPARATION, 3));
    modelShips.push_back(Spaceship(2, WIDTH - 150, 5 * SEPARATION, 4));
}

void Map::Peace()
{
    terranBase.Peace(martianBase); // Each of the bases' peace functions.
    martianBase.Peace(terranBase);
}

void Map::NextLevel()
{
    // Self-explanatory functions that finalize any upgrade purchases, gather interest for unspent resources, and earn resources for making through the round.
    terranBase.FinalizePurchase();
    terranBase.GatherInterest();
    terranBase.EarnResources(GetBonusResourcesByLevel(level));
    martianBase.FinalizePurchase();
    martianBase.GatherInterest();
    martianBase.EarnResources(GetBonusResourcesByLevel(level));

    terranBase.NextLevel(); // NextLevel functions for the bases.
    martianBase.NextLevel();
    ++level; // Advance the level counter.
}

int Map::WarPeriodic()
{
    // Base periodic functions.
    terranBase.Periodic(martianBase.GetShips(), martianBase.GetBullets());
    martianBase.Periodic(terranBase.GetShips(), terranBase.GetBullets());
    if (terranBase.ShieldDown()) // If the shield is down.
        terranBase.GiveLoot(martianBase); // Give loot to the other base.
    if (martianBase.ShieldDown())
        martianBase.GiveLoot(terranBase);

    // Draw bases, ships, and bullets for each side.
    terranBase.DrawBase();
    martianBase.DrawBase();
    terranBase.DrawShips();
    martianBase.DrawShips();
    terranBase.DrawBullets();
    martianBase.DrawBullets();

    if (!terranBase.IsAlive()) // If martians won.
        return 2; // Return 2.
    else if (!martianBase.IsAlive()) // If terrans won.
        return 1; // Return 1.
    else
        return 0; // Otherwise return 0.
}

void Map::ControlTerranFlagship(char c)
{
    // Depending on the character command given, different command functions are used.
    switch (c)
    {
    case 'u': // If 'u', then move the ships up... etc.
        terranBase.FlagshipMoveUp();
        break;
    case 'd':
        terranBase.FlagshipMoveDown();
        break;
    case 'l':
        terranBase.FlagshipMoveLeft();
        break;
    case 'r':
        terranBase.FlagshipMoveRight();
        break;
    case 'f':
        terranBase.FlagshipFire();
        break;
    }
}

void Map::ControlMartianFlagship(char c)
{
    // Depending on the character command given, different command functions are used.
    switch (c)
    {
    case 'u': // If 'u', then move the ships up... etc.
        martianBase.FlagshipMoveUp();
        break;
    case 'd':
        martianBase.FlagshipMoveDown();
        break;
    case 'l':
        martianBase.FlagshipMoveLeft();
        break;
    case 'r':
        martianBase.FlagshipMoveRight();
        break;
    case 'f':
        martianBase.FlagshipFire();
        break;
    }
}

void Map::TerranSelectionUp()
{
    // The selection can only go up to 5.
    if (t < 4)
    {
        ++t; // Increment selection.
        terranSelectionShip.MoveY(SEPARATION); // Change the selection ships's position.
    }
}

void Map::TerranSelectionDown()
{
    // The selection can only go down to 0.
    if (t > 0)
    {
        --t; // Decrement selection.
        terranSelectionShip.MoveY(-1 * SEPARATION);; // Change the selection ships's position.
    }
}

// Same as terrans.
void Map::MartianSelectionUp()
{
    if (m < 4)
    {
        ++m;
        martianSelectionShip.MoveY(SEPARATION);
    }
}

void Map::MartianSelectionDown()
{
    if (m > 0)
    {
        --m;
        martianSelectionShip.MoveY(-1 * SEPARATION);
    }
}

void Map::PeacePeriodic()
{
    // Draw labels.
    glColor3f(0.6, 0.6, 1);
    DrawString("Terran", 100, HEIGHT - 50);
    glColor3f(1, 0, 0);
    DrawString("Martian", WIDTH - 150, HEIGHT - 50);
    // Draw resources remaining.
    glColor3f(0, 0.7, 0);
    DrawString(IntToString(terranBase.GetResources()), 180, HEIGHT - 50);
    DrawString(IntToString(martianBase.GetResources()), WIDTH - 210, HEIGHT - 50);
    // Draw terran upgrade numbers.
    glColor3f(0, 0, 0.7);
    for (int i = 0; i != 5; ++i)
    {
        DrawString(IntToString(terranBase.GetUpgrade(i)), 200, SEPARATION * (i + 1));
        DrawString('+' + IntToString(terranBase.GetUnfinalizedUpgrade(i)), 250, SEPARATION * (i + 1));
    }
    // Draw martian upgrade numbers.
    glColor3f(0.7, 0, 0);
    for (int i = 0; i != 5; ++i)
    {
        DrawString(IntToString(martianBase.GetUpgrade(i)), WIDTH - 230, SEPARATION * (i + 1));
        DrawString('+' + IntToString(martianBase.GetUnfinalizedUpgrade(i)), WIDTH - 280, SEPARATION * (i + 1));
    }
    // Draw model ships.
    for (vector<Spaceship>::const_iterator it = modelShips.begin(); it != modelShips.end(); ++it)
        it->Draw();
    // Draw descriptions.
    glColor3f(0.9, 0.9, 1);
    for (size_t i = 0; i != 5; ++i)
        DrawString(DESCRIPTIONS[i], WIDTH / 2 - 100, (i + 1) * SEPARATION);
    // Draw selection ships.
    terranSelectionShip.Draw();
    martianSelectionShip.Draw();
}
