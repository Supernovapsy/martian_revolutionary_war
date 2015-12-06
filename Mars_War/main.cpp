// Wen Li ICS4U1 Performance Task
// Martian Revolutionary War
// Mr.Wilford
// Handed in with permission on 25/06/12
#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// The necessary headers for this file.
#include <cstddef>
#include <ctime>
#include "Spaceship.h"
#include "Global.h"
#include "Base.h"
#include "Map.h"

using namespace std;

// Minimum time for each round and the chance of a round ending after that time has ended.
const int LEVEL_INTERVAL = 25, PEACE_CHANCE = 5000; // Interval is in seconds, and chance is 1/n.

// Arrays to store the state of each key on the keyboard and their previous states.
bool normalKeys[256], specialKeys[256], prevNormalKeys[256], prevSpecialKeys[256];

Map map; // Initialize the game map.
const size_t randomStarN = 10; // The number of random stars in the background.
// The x and y values of the random stars, and the end state of the game (1 means Terrans won and 0 means Martians won).
int randX[randomStarN], randY[randomStarN], endState = 0;
// Booleans for whether or not it is peace, the beginning, end, and whether or not the game is paused.
bool peace = true, begin = true, end = false, pause = false;
// The starting time of each round stored in a timer.
time_t startTime;

// The line spacing between each line in the crawl text and their move speed.
const float LINE_SPACING = 32, TEXT_SPEED = 0.25;
// The height of the text (which starts at the bottom).
float textHeight = 0;
// The number of lines for each crawl text.
size_t crawlTextSizes[] = {20, 6, 5};

// The crawl text for the beginning of the game.
const char* const crawlTextBegin[] =
{
    "A long time ago in a galaxy far,",
    "far away....", "", "", "", "",
    "MARTIAN REVOLUTIONARY WAR", "", "", "",
    "It is a period of civil war.",
    "Rebel spaceships, striking from a hidden base, surprised",
    "fighters in transport, sparking the Martian's Revolution",
    "against the Terran Empire. The terrans, on the other hand,",
    "are furious that the development of humans in the past",
    "centuries are being hindered by a group of disloyal leaders",
    "who cite that freedom is not given to the hard-working citizens",
    "of Mars. It is time to take your side, and join the loyalists",
    "or the revolutionaries, and preserve or change the ideal",
    "of every human being in the galaxy."
};

// The crawl text for the terran ending of the game.
const char* const crawlTextTerranEnd[] =
{
    "The war has ended. The people on both sides are nearly shattered.",
    "But it is the terrans who have shown that loyalty and unity are",
    "the more powerful combination, that the prosperity of the",
    "hegemony under the Empire will rival the glory that was achieved",
    "under the Romans, and whose achievement and progress will leave a",
    "prominent mark in the entirety of human history."
};

// The crawl text for the martian ending of the game.
const char* const crawlTextMartianEnd[] =
{
    "The war has ended. The people on both sides are nearly shattered.",
    "But it is the martians who have shown that liberty and freedom",
    "are the most enduring qualities that guide humans on their path,",
    "and that the shrouds of oppression will always one day be cleared.",
    "The Martians rejoice for their independence as a new nation."
};

// The following are a series of functions which update the state of each key on the keyboard.
void NormalKeyDown(unsigned char key, int x, int y)
{
    normalKeys[key] = true;
}

void NormalKeyUp(unsigned char key, int x, int y)
{
    normalKeys[key] = false;
}

void SpecialKeyDown(int key, int x, int y)
{
    specialKeys[key] = true;
}

void SpecialKeyUp(int key, int x, int y)
{
    specialKeys[key] = false;
}

// Key operations for when the upgrades are being purchased.
void PeaceKeyOperations()
{
    // If the w key is pressed and it wasn't pressed before.
    if ((normalKeys['w'] && !prevNormalKeys['w']) || (normalKeys['W'] && !prevNormalKeys['W']))
        map.TerranSelectionUp();
    else if ((normalKeys['s'] && !prevNormalKeys['s']) || (normalKeys['S'] && !prevNormalKeys['S']))
        map.TerranSelectionDown();
    if ((normalKeys['d'] && !prevNormalKeys['d']) || (normalKeys['D'] && !prevNormalKeys['D']))
        map.TerranSelectionConfirm();
    else if ((normalKeys['a'] && !prevNormalKeys['a']) || (normalKeys['A'] && !prevNormalKeys['A']))
        map.TerranSelectionUndo();

    if (specialKeys[GLUT_KEY_UP] && !prevSpecialKeys[GLUT_KEY_UP])
        map.MartianSelectionUp();
    else if (specialKeys[GLUT_KEY_DOWN] && !prevSpecialKeys[GLUT_KEY_DOWN])
        map.MartianSelectionDown();
    if (specialKeys[GLUT_KEY_RIGHT] && !prevSpecialKeys[GLUT_KEY_RIGHT])
        map.MartianSelectionConfirm();
    else if (specialKeys[GLUT_KEY_LEFT] && !prevSpecialKeys[GLUT_KEY_LEFT])
        map.MartianSelectionUndo();

    // If the enter key is pressed during peace time.
    if (normalKeys[13] && !prevNormalKeys[13])
    {
        // It's war now!
        peace = false;
        map.NextLevel(); // NextLevel function.
        startTime = time(0); // Reset the starting time.
    }
}

// Key operations for when the war is going on.
void WarKeyOperations()
{
    // These control the flagship.
    if (normalKeys['w'] || normalKeys['W'])
        map.ControlTerranFlagship('u');
    else if (normalKeys['s'] || normalKeys['S'])
        map.ControlTerranFlagship('d');
    if (normalKeys['a'] || normalKeys['A'])
        map.ControlTerranFlagship('l');
    else if (normalKeys['d'] || normalKeys['D'])
        map.ControlTerranFlagship('r');
    if (normalKeys['1'])
        map.ControlTerranFlagship('f');

    if (specialKeys[GLUT_KEY_UP])
        map.ControlMartianFlagship('u');
    else if (specialKeys[GLUT_KEY_DOWN])
        map.ControlMartianFlagship('d');
    if (specialKeys[GLUT_KEY_LEFT])
        map.ControlMartianFlagship('l');
    else if (specialKeys[GLUT_KEY_RIGHT])
        map.ControlMartianFlagship('r');
    if (normalKeys['/'])
        map.ControlMartianFlagship('f');
}

// The main key control function.
void ProcessKeys()
{
    // Exit when ESC is presed.
    if (normalKeys[27])
        exit(0);
    // Pause when spacebar is pressed.
    if (normalKeys[32] && !prevNormalKeys[32])
        pause = !pause;

    // If not paused.
    if (!pause)
    {
        // If the game is not beginning or ending.
        if (!begin && !end)
        {
            if (peace)
                PeaceKeyOperations(); // Do peace operations when in peace.
            else
                WarKeyOperations(); // Do war operations when in war.
        }
        // When the game is beginning and enter is pressed, the crawl text is skipped.
        if (begin && normalKeys[13])
            begin = false;
    }

    // This updates whether or not the key was previously pressed.
    for (size_t i = 0; i != 256; ++i)
    {
        prevNormalKeys[i] = normalKeys[i];
        prevSpecialKeys[i] = specialKeys[i];
    }
}

// Function to draw crawl text.
void DrawCrawlText(int textN)
{
    glColor3f(1, 1, 0); // Yellow colour.
    // Depending on which text to display.
    switch (textN)
    {
    // The crawl text is displayed through for loops that go through each element in the array that stores the text.
    case 0:
        for (size_t i = 0; i != crawlTextSizes[textN]; ++i)
        {
            // The text "A long time ago in a galaxy far, far away...." needs to be blue :p.
            if (i == 0 || i == 1)
                glColor3f(0.2, 0.4, 1);
            else
                glColor3f(1, 1, 0);
            // This uses a user-defined function to draw the text, spaced out by LINE_SPACING.
            DrawString(crawlTextBegin[i], 250, (crawlTextSizes[textN] - i) * LINE_SPACING - crawlTextSizes[textN] * LINE_SPACING + textHeight);
        }
        break;
    case 1:
        for (size_t i = 0; i != crawlTextSizes[textN]; ++i)
            DrawString(crawlTextTerranEnd[i], 230, (crawlTextSizes[textN] - i) * LINE_SPACING - crawlTextSizes[textN] * LINE_SPACING + textHeight);
        break;
    case 2:
        for (size_t i = 0; i != crawlTextSizes[textN]; ++i)
            DrawString(crawlTextMartianEnd[i], 230, (crawlTextSizes[textN] - i) * LINE_SPACING - crawlTextSizes[textN] * LINE_SPACING + textHeight);
        break;
    }
    // If the text is out of the screen.
    if (textHeight - crawlTextSizes[textN] * LINE_SPACING > HEIGHT)
    {
        // Turn the boolean to false if begin is true.
        if (begin)
            begin = false;
        // Otherwise the game has ended, so exit.
        else
            exit(0);
    }
}

// This draws all the stars.
void DrawStars()
{
    // The following are the default stars.
    DrawStar(8, 178, 50);
    DrawStar(12, 225, 125);
    DrawStar(18, 398, 370);
    DrawStar(24, 608, 604);
    DrawStar(16, 489, 523);
    DrawStar(32, 418, 472);
    DrawStar(17, 518, 60);
    DrawStar(21, 498, 652);
    DrawStar(26, 348, 153);
    DrawStar(22, 841, 560);
    DrawStar(14, 196, 436);
    DrawStar(16, 741, 190);

    // These are the random stars.
    for (size_t i = 0; i != randomStarN; ++i)
        DrawStar(16, randX[i], randY[i]);
}

// Function to be called periodically.
void RenderScene()
{
    ProcessKeys(); // Process keyboard.

    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);

    // If the game is paused.
    if (pause)
    {
        // Notify the user and don't do anything else.
        glColor3f(0.6, 0.8, 1);
        DrawString("GAME PAUSED", WIDTH / 2 - 50, HEIGHT / 2);
    }
    // If the game is beginning.
    else if (begin)
    {
        // Draw and advance the crawl text.
        DrawCrawlText(0);
        textHeight += TEXT_SPEED;
    }
    // If the game is ending.
    else if (end)
    {
        // Draw and advance the crawl text.
        DrawCrawlText(endState);
        textHeight += TEXT_SPEED;
    }
    // If the game is going on.
    else if (!end)
    {
        if (peace)
            map.PeacePeriodic(); // Use the peace function if it is peace.
        // Else there is a random chance of peace occurring.
        else if (difftime(time(0), startTime) > LEVEL_INTERVAL && randn(PEACE_CHANCE) == 0)
        {
            map.Peace(); // Call the initial peace function.
            peace = true; // Make the boolean true.
        }
        // Otherwise.
        else
        {
            DrawStars(); // Draw the background.
            endState = map.WarPeriodic(); // Call the war function, and store its return value.
        }
    }

    // If the return value is not 0, then the game has ended.
    if (!end && endState != 0)
    {
        end = true; // Turn the boolean to be true.
        textHeight = 0; // Reset the crawl text height.
    }

    glutSwapBuffers(); // Put on the buffer.
}

// Function to reshape the screen.
void Reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION); // Loads the projection matrix.
    glLoadIdentity(); // Loads the original matrix.
    gluOrtho2D(0, WIDTH, 0, HEIGHT); // Set the 2D screen.
    glMatrixMode(GL_MODELVIEW); // Change back to the modelview matrix.
}

// Init function for the program.
void init()
{
    // Set all of the functions into GLUT.
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(Reshape);
    glutIdleFunc(RenderScene);

    glutIgnoreKeyRepeat(0);
    glutKeyboardFunc(NormalKeyDown);
    glutKeyboardUpFunc(NormalKeyUp);
    glutSpecialFunc(SpecialKeyDown);
    glutSpecialUpFunc(SpecialKeyUp);

    // Initialize the random positions of the random stars.
    for (size_t i = 0; i != randomStarN; ++i)
    {
        randX[i] = randn(WIDTH);
        randY[i] = randn(HEIGHT);
    }
}

// Main function.
int main(int argc, char** argv)
{
    glutInit(&argc, argv); // Initialize.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Double buffering, RGB colouring.
    glutInitWindowSize(WIDTH, HEIGHT); // Set window size.
    glutInitWindowPosition(100, 100); // Position too.
    glutCreateWindow("Martian Revolutionary War"); // Create the window.

    srand(time(0)); // Seed random number generator.
    init(); // Initialize GLUT functions.

    glMatrixMode(GL_PROJECTION); // Loads the projection matrix.
    glLoadIdentity(); // Loads the original matrix.
    gluOrtho2D(0, WIDTH, 0, HEIGHT); // Set the 2D screen.
    glMatrixMode(GL_MODELVIEW); // Change back to the modelview matrix.

    startTime = time(0); // Store the starting time.

    glutMainLoop(); // Call the main loop.
    return 1;
}
