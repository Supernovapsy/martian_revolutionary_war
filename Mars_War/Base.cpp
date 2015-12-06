#include "Base.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>

#define _USE_MATH_DEFINES

#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

// Definitions for the constants.
const int Base::BASE_SPAWNING_CHANCES[5] = {1, 1500, 2000, 4000, 5000};
const int Base::UPGRADE_COSTS[5] = {300, 250, 250, 400, 150};
const int Base::SHIP_KILL_BONUSES[5] = {100, 2, 20, 40, 10};

bool EraseShipPred(const Spaceship& s)
{
    return s.GetType() == 1; // This predicate returns true if the spaceship in question is a regular ship.
}

// Some fields are initialized straight in the constructor initializer, some depend on more things and are initialized below.
Base::Base(int s, float pos_x, float pos_y): side(s), x(pos_x), y(pos_y), hp(BASE_HP), resources(INITIAL_RESOURCES), factoryLevel(0), flagshipLevel(0), flagship_p(0), flagshipAlive(true), doNotSpawnFlagship(false), resourcesForOtherBase(0)
{
    radius = HEIGHT * std::sqrt(2) / 2; // The radius is this due to some math calculations that I can't remember when I commented this. (This makes it so that a quarter of a circle with this radius JUST touches the upper and lower boundaries of the screen).
    shipsAvailable[2] = 3; // 3 Fighters in the beginning.
    shipsAvailable[3] = shipsAvailable[4] = 0; // None of the others.
    shield1.down = shield2.down = false; // Shields are up in the beginning.
    shield1.hp = SHIELD_1_HP; // Initialize according to constant.
    shield2.hp = SHIELD_2_HP;
    shield1.distance = SHIELD_1_DISTANCE; // Initialize.
    shield2.distance = SHIELD_2_DISTANCE;

    // Zero all these values, as the player hasn't spent any resources yet.
    for (size_t i = 0; i != 5; ++i)
        unfinalizedPurchases[i] = 0;
}

bool Base::ShieldDown()
{
    if (!shield1.down && shield1.hp <= 0) // If the first shield is down.
        return (shield1.down = true); // Set the boolean is true and return it.
    else if (!shield2.down && shield2.hp <= 0) // If the second is down.
        return (shield2.down = true); // Do the same.
    return false; // Otherwise no shield was downed.
}

int Base::GetShipsAvailable(int n) const
{
    if (n < 2 || n > 4) // The player can only buy fighters, bombers, and kamikaze ships.
        throw domain_error("Selected ship does not exist; ships available ranges from 2-4.");
    return shipsAvailable[n]; // Return the number of ships in stock.
}

int Base::GetUpgrade(int l) const
{
    switch (l) // Get the number of each current upgrade.
    {
    case 0:
        return flagshipLevel;
    case 1:
        return factoryLevel;
    case 2:
        return shipsAvailable[2];
    case 3:
        return shipsAvailable[3];
    case 4:
        return shipsAvailable[4];
    default: // Throw error if out of range.
        throw domain_error("Upgrade does not exist; upgrades range from 0-4.");
    }
}

int Base::GetUnfinalizedUpgrade(int l) const
{
    if (l < 0 || l >= 5) // Upgrades only range from 0-4.
        throw domain_error("Selected unfinalized upgrade does not exist; these upgrades range from 0-4.");
    return unfinalizedPurchases[l]; // Get the number of each unfinalized upgrade.
}

void Base::MakePurchase(int selection)
{
    if (selection < 0 || selection >= 5) // Upgrades only range from 0-4.
        throw domain_error("Selected purchase does not exist.");
    // You can't buy what money you don't have, and you can't buy over the upgrade limit.
    if (UPGRADE_COSTS[selection] <= resources && unfinalizedPurchases[selection] + GetUpgrade(selection) < UPGRADE_LIMIT)
    {
        resources -= UPGRADE_COSTS[selection]; // Subtract resources.
        ++unfinalizedPurchases[selection]; // Add to the unfinalized upgrades.
    }
}

void Base::UndoPurchase(int selection)
{
    if (selection < 0 || selection >= 5) // Upgrades only range from 0-4.
        throw domain_error("Selected purchase does not exist.");
    // You can't return what you didn't buy.
    if (unfinalizedPurchases[selection] > 0)
    {
        resources += UPGRADE_COSTS[selection]; // Add to resources.
        --unfinalizedPurchases[selection]; // Subtract from the unfinalized upgrades.
    }
}

void Base::FinalizePurchase()
{
    // Add the unfinalized purchases to the finalized upgrades.
    flagshipLevel += unfinalizedPurchases[0];
    factoryLevel += unfinalizedPurchases[1];
    shipsAvailable[2] += unfinalizedPurchases[2];
    shipsAvailable[3] += unfinalizedPurchases[3];
    shipsAvailable[4] += unfinalizedPurchases[4];
    // And reset them.
    for (size_t i = 0; i != 5; ++i)
        unfinalizedPurchases[i] = 0;
}

// This gets called when a shield is down.
void Base::GiveLoot(Base& other)
{
    // Calculate the loot through random chance.
    int loot = (randn(MAX_LOOT_PERCENTAGE - MIN_LOOT_PERCENTAGE) + MIN_LOOT_PERCENTAGE + 1) * resources * 0.01; // The loot is MIN-MAX% of the other player's total resources
    resources -= loot; // Lose the loot.
    other.EarnResources(loot); // Give the loot.
}

// This gets called when peace begins.
void Base::Peace(Base& other)
{
    // Erase all regular ships from the list.
    ships.erase(remove_if(ships.begin(), ships.end(), ::EraseShipPred), ships.end());
    // Putting all of the special ships back.
    for (int i = 2; i != 5; ++i)
    {
        for (list<Spaceship>::iterator it = ships.begin(); it != ships.end(); ++it)
        {
            if (it->GetType() == i)
            {
                it = ships.erase(it); // Erase it from the map.
                ++shipsAvailable[i]; // Add it to the stock.
            }
        }
    }
    bullets.clear(); // Clear all the bullets.
    other.EarnResources(resourcesForOtherBase); // Give the bonus for destroying ships to the other base.
    resourcesForOtherBase = 0; // And zero it.
}

void Base::NextLevel()
{
    // If the ship is not to be spawned.
    if (doNotSpawnFlagship)
        doNotSpawnFlagship = false; // Allow it to be spawned next round.
    // If the ship is to be spawned.
    else
        SpawnFlagship(); // Then spawn it.
}

void Base::Periodic(list<Spaceship>& enemyShips, list<Bullet>& enemyBullets)
{
    // Spawning a specially constructed ship.
    for (size_t i = 2; i != 5; ++i)
    {
        // If by chance it should be spawned.
        if (shipsAvailable[i] > 0 && randn(GetChanceFromLevelThroughBaseChance(BASE_SPAWNING_CHANCES[i], flagshipLevel)) == 0)
        {
            // Put it on the map.
            ships.push_back(Spaceship(side, side == 1 ? 0 : WIDTH, randn(HEIGHT), i));
            --shipsAvailable[i]; // Take a ship away from the stockpile.
        }
    }
    // Spawning a regular ship from the factory.
    if (randn(GetChanceFromLevelThroughBaseChance(BASE_SPAWNING_CHANCES[1], factoryLevel)) == 0)
        ships.push_back(Spaceship(side, side == 1 ? 0 : WIDTH, randn(HEIGHT), 1));

    // Going through all of the bullets.
    for (list<Bullet>::iterator it = bullets.begin(); it != bullets.end(); ++it)
    {
        it->Move(); // Move it.
        if (it->OutOfBounds()) // If it is out of bounds, erase it.
            it = bullets.erase(it);
    }

    // Going through all of the ships.
    for (list<Spaceship>::iterator it = ships.begin(); it != ships.end(); ++it)
    {
        if (!it->IsAlive()) // If the ship died.
        {
            if (it->GetType() == 0) // If the ship was a flagship.
            {
                flagshipAlive = false; // It's not alive.
                doNotSpawnFlagship = true; // Do not spawn it for the next round.
            }
            resourcesForOtherBase += SHIP_KILL_BONUSES[it->GetType()]; // Add the corresponding bonus to the total bonus to be given.
            if ((it = ships.erase(it)) == ships.end()) // After erasing the ship, if the iterator has reached the end of the list, break the loop.
                break;
        }
        if (it->GetType() == 4) // If ship was a kamikaze.
        {
            // Depending on whether it's on the left or right side, collision detection is different.
            if (side == 1 && it->GetTailX() < 0)
                it->HitBase();
            else if (side == 2 && it->GetTailX() > WIDTH)
                it->HitBase();
        }
        it->Periodic(); // Call the ship's periodic function.
        if (it->GetType() != 0 && it->ReadyToFire()) // If the ship is A.I. controlled and it's ready to fire.
        {
            it->Fire(); // Fire!
            // Construct a bullet from the ship.
            bullets.push_back(Bullet(side, it->GetX() - Bullet::LENGTHS[it->GetType()], it->GetY(), it->GetType()));
        }
    }

    /// Collision detection.
    // Looping through the bullets.
    for (list<Bullet>::iterator it = bullets.begin(); it != bullets.end(); ++it)
    {
        // Looping through the ships.
        for (list<Spaceship>::iterator it2 = enemyShips.begin(); it2 != enemyShips.end(); ++it2)
        {
            // Check for detection.
            if (it2->IsPointOnShip(it->GetXLeft(), it->GetY()) || it2->IsPointOnShip(it->GetXRight(), it->GetY()))
            {
                it2->DamagedBy(it->GetPower()); // Register the damage.
                if ((it = bullets.erase(it)) == bullets.end()) // After erasing the bullet, if the iterator has reached the end of the list, break the loop.
                    break;
            }
        }
    }
    // Detection against shield and base.
    if (IsAlive()) // If the base is still alive.
    {
        // Looping through the enemy bullets.
        for (list<Bullet>::iterator it = enemyBullets.begin(); it != enemyBullets.end(); ++it)
        {
            float distanceToOrigin; // Distance from bullet to origin of the circle that defines the arc of the shield or planet.

            // Explaining exactly how the code below works is too tedious, so I'll just summarize what it does.
            // It calculates the distance from the origin to the bullet, and compares it with the radius of the shield
            // or base. If the distance for the bullet is less, then the bullet has hit the shield.
            if (side == 1)
            {
                if (!shield1.down)
                    distanceToOrigin = sqrt(pow(it->GetXLeft() - (-radius + SHIELD_1_DISTANCE), 2) + pow(it->GetY() - y, 2));
                else if (!shield2.down)
                    distanceToOrigin = sqrt(pow(it->GetXLeft() - (-radius + SHIELD_2_DISTANCE), 2) + pow(it->GetY() - y, 2));
                else
                    distanceToOrigin = sqrt(pow(it->GetXLeft() - (-radius + BASE_DISTANCE), 2) + pow(it->GetY() - y, 2));
            }
            else if (side == 2)
            {
                if (!shield1.down)
                    distanceToOrigin = sqrt(pow(it->GetXRight() - (x + radius - SHIELD_1_DISTANCE), 2) + pow(it->GetY() - y, 2));
                else if (!shield2.down)
                    distanceToOrigin = sqrt(pow(it->GetXRight() - (x + radius - SHIELD_2_DISTANCE), 2) + pow(it->GetY() - y, 2));
                else
                    distanceToOrigin = sqrt(pow(it->GetXRight() - (x + radius - BASE_DISTANCE), 2) + pow(it->GetY() - y, 2));
            }
            if (distanceToOrigin <= radius)
            {
                if (!shield1.down) // If the first shield is still up.
                    shield1.hp -= it->GetBasePower(); // Subtract from its hp.
                else if (!shield2.down) // If the second shield is still up.
                    shield2.hp -= it->GetBasePower(); // Subtract from its hp.
                else // Otherwise subtract from the planet's hp.
                    hp -= it->GetPower();
                if ((it = enemyBullets.erase(it)) == enemyBullets.end()) // Have to do this to not use an invalid pointer.
                    break;
            }
        }
        // This does the same as above, except it checks through each of the enemy's kamikaze ships for a ship collision.
        for (list<Spaceship>::iterator it = enemyShips.begin(); it != enemyShips.end(); ++it)
        {
            if (it->GetType() == 4)
            {
                float distanceToOrigin;
                if (side == 1)
                {
                    if (!shield1.down)
                        distanceToOrigin = sqrt(pow(it->GetX() - (-radius + SHIELD_1_DISTANCE), 2) + pow(it->GetY() - y, 2));
                    else if (!shield2.down)
                        distanceToOrigin = sqrt(pow(it->GetX() - (-radius + SHIELD_2_DISTANCE), 2) + pow(it->GetY() - y, 2));
                    else
                        distanceToOrigin = sqrt(pow(it->GetX() - (-radius + BASE_DISTANCE), 2) + pow(it->GetY() - y, 2));
                }
                else if (side == 2)
                {
                    if (!shield1.down)
                        distanceToOrigin = sqrt(pow(it->GetX() - (x + radius - SHIELD_1_DISTANCE), 2) + pow(it->GetY() - y, 2));
                    else if (!shield2.down)
                        distanceToOrigin = sqrt(pow(it->GetX() - (x + radius - SHIELD_2_DISTANCE), 2) + pow(it->GetY() - y, 2));
                    else
                        distanceToOrigin = sqrt(pow(it->GetX() - (x + radius - BASE_DISTANCE), 2) + pow(it->GetY() - y, 2));
                }
                if (distanceToOrigin <= radius)
                {
                    if (!shield1.down)
                        shield1.hp -= Spaceship::KAMIKAZE_DAMAGE;
                    else if (!shield2.down)
                        shield2.hp -= Spaceship::KAMIKAZE_DAMAGE;
                    else
                        hp -= Spaceship::KAMIKAZE_DAMAGE;
                    it->HitBase();
                }
            }
        }
    }
}

void Base::FlagshipFire()
{
    // If the flagship can fire.
    if (flagshipAlive && flagship_p->ReadyToFire())
    {
        flagship_p->Fire(); // Fire it.
        // Create a bullet from its position and the flagship's level.
        bullets.push_back(Bullet(side, flagship_p->GetX() - Bullet::LENGTHS[0], flagship_p->GetY(), 0, flagship_p->GetLevel()));
    }
}

void Base::SpawnFlagship()
{
    // Loopin through all of the player's ships.
    for (list<Spaceship>::iterator it = ships.begin(); it != ships.end(); ++it)
    {
        // If the flagship is found, erase it and replace it.
        if (it->GetType() == 0)
            it = ships.erase(it);
    }
    ships.push_back(Spaceship(side, side == 1 ? 0 : WIDTH, randn(HEIGHT), 0, flagshipLevel)); // Creates a new flagship.
    list<Spaceship>::iterator shipsIterator = ships.end();
    flagship_p = --shipsIterator; // Store the address of the new flagship into the flagship pointer.
    flagshipAlive = true; // Make the boolean true.
}

void Base::DrawShips() const
{
    for (list<Spaceship>::const_iterator it = ships.begin(); it != ships.end(); ++it)
        it->Draw(); // Draw the ships.
}

void Base::DrawBullets() const
{
    for (list<Bullet>::const_iterator it = bullets.begin(); it != bullets.end(); ++it)
        it->Draw(); // Draw the bullets.
}

void Base::DrawBase() const
{
    const int k = side * 2 - 3; // This constant is either 1 or -1, and adjusts the position of the shields depending on which side it is on.
    const float baseX = x + k * radius; // This number is the base position of the shield if there was not any distance between it and the position of the base.

    glColor4f(0, 1, 1, 0.2); // Colour of shield.
    float colourIntensity = -(hp / BASE_HP) * 4 / 5 - 0.2 + 1; // Colour intensity of base.
    // For the left side (Earth)
    if (k < 0)
    {
        // For explanation of how the shield is drawn, please see the RAT Reflection word document.
        // Both the shields and base are drawn in the same way.

        // First Shield
        const float top = 2 * M_PI / 8.0, bottom = -2 * M_PI / 8.0, increment = 2 * M_PI / 200;
        float aLeft = top, aRight = top;
        if (!shield1.down)
        {
            glBegin(GL_QUAD_STRIP);
                while (aLeft >= bottom)
                {
                    glVertex2f(radius * cos(aLeft) + baseX + SHIELD_1_DISTANCE - (shield1.hp / SHIELD_1_HP) * SHIELD_WIDTH, radius * sin(aLeft) + y);
                    glVertex2f(radius * cos(aRight) + baseX + SHIELD_1_DISTANCE, radius * sin(aRight) + y);
                    aLeft -= increment;
                    aRight -= increment;
                }
            glEnd();
        }
        // Second Shield
        aLeft = top, aRight = top;
        if (!shield2.down)
        {
            glBegin(GL_QUAD_STRIP);
                while (aLeft >= bottom)
                {
                    glVertex2f(radius * cos(aLeft) + baseX + SHIELD_2_DISTANCE - (shield2.hp / SHIELD_2_HP) * SHIELD_WIDTH, radius * sin(aLeft) + y);
                    glVertex2f(radius * cos(aRight) + baseX + SHIELD_2_DISTANCE, radius * sin(aRight) + y);
                    aLeft -= increment;
                    aRight -= increment;
                }
            glEnd();
        }
        // The base.
        if (IsAlive())
        {
            glColor4f(colourIntensity, colourIntensity, 1, 0.2);
            aLeft = top, aRight = top;
            glBegin(GL_QUAD_STRIP);
                while (aLeft >= bottom)
                {
                    glVertex2f(radius * cos(aLeft) + baseX + BASE_DISTANCE - (radius - 1), radius * sin(aLeft) + y);
                    glVertex2f(radius * cos(aRight) + baseX + BASE_DISTANCE, radius * sin(aRight) + y);
                    aLeft -= increment;
                    aRight -= increment;
                }
            glEnd();
        }
    }
    // For the right side (Mars)
    else if (k > 0)
    {
        // First Shield
        const float top = 3 * 2 * M_PI / 8.0, bottom = 5 * 2 * M_PI / 8.0, increment = 2 * M_PI / 200;
        float aLeft = top, aRight = top;
        if (!shield1.down)
        {
            glBegin(GL_QUAD_STRIP);
                while (aLeft <= bottom)
                {
                    glVertex2f(radius * cos(aLeft) + baseX - SHIELD_1_DISTANCE, radius * sin(aLeft) + y);
                    glVertex2f(radius * cos(aRight) + baseX - SHIELD_1_DISTANCE + (shield1.hp / SHIELD_1_HP) * SHIELD_WIDTH, radius * sin(aRight) + y);
                    aLeft += increment;
                    aRight += increment;
                }
            glEnd();
        }
        // Second Shield
        if (!shield2.down)
        {
            aLeft = top, aRight = top;
            glBegin(GL_QUAD_STRIP);
                while (aLeft <= bottom)
                {
                    glVertex2f(radius * cos(aLeft) + baseX - SHIELD_2_DISTANCE, radius * sin(aLeft) + y);
                    glVertex2f(radius * cos(aRight) + baseX - SHIELD_2_DISTANCE + (shield2.hp / SHIELD_2_HP) * SHIELD_WIDTH, radius * sin(aRight) + y);
                    aLeft += increment;
                    aRight += increment;
                }
            glEnd();
        }
        // The base.
        if (IsAlive())
        {
            glColor4f(1, colourIntensity, colourIntensity, 0.2); // The only differences betweent the two are the colour...
            aLeft = top, aRight = top;
            glBegin(GL_QUAD_STRIP);
                while (aLeft <= bottom)
                {
                    glVertex2f(radius * cos(aLeft) + baseX - BASE_DISTANCE, radius * sin(aLeft) + y);
                    glVertex2f(radius * cos(aRight) + baseX - BASE_DISTANCE + (radius - 1), radius * sin(aRight) + y); // and that the arc is a filled arc.
                    aLeft += increment;
                    aRight += increment;
                }
            glEnd();
        }
    }
}
