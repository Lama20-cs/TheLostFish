// ========================================
// Computer Graphics Project - The Lost Fish
// Course: CS2206
// Group : 1
// ========================================

#define _CRT_SECURE_NO_WARNINGS
#define GL_SILENCE_DEPRECATION
#include <GL/glut.h>
#include <cmath>
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

// ========================================
// Global Variables
// ========================================

float parentsX = 0.0f;         // how far the parents have moved horizontally
int currentScene = 1;          // which scene we are showing right now

float zoomLevel = 1.0f;

// bubble Y positions change over time, X positions stay fixed
float bubY[6] = { -0.8f, -0.5f, -0.3f, -0.7f, -0.4f, -0.6f };
float bubX[6] = { -0.9f, -0.7f, -0.5f, -0.3f, -0.1f, 0.1f };

// used to track which step of scene 4 and scene 8 we are in
int scene4State = 0;
int scene8State = 0;

// scene 8 positions
float scene8FishX = -1.25f;
float scene8FishY = -0.05f;
float scene8ParentX = 1.4f;
float scene8Timer = 0;

int smallFishState = 0;

// position variables for the small fish and other characters
float smallFishX = -1.0f;
float smallFishY = 0.0f;
float parentFishX = -0.5f;
float sharkX = 1.5f;
float fishX = 0.8f;

bool start = true;
int delay = 0;

// scene 5 fish variables
float scene5FishX = 0.0f;
bool scene5FishFacingLeft = true;

// texture handles
GLuint myTexture1;
char cavePath[] = "cava2.bmp";

GLuint myTexture2;
char rockPath[] = "rock.bmp";

// function declarations
void drawScene1();
void drawScene2();
void drawScene3();
void drawScene4();
void drawScene5();
void drawScene6();
void drawFloor();
void drawCave();
void drawBubbles();
void drawScene7();
void drawScene8();
void drawScene7Background();


// reads a BMP file from disk and creates an OpenGL texture from it
// returns the texture ID, or 0 if the file could not be opened
GLuint LoadTexture(const char* filename) {

    FILE* file = fopen(filename, "rb");

    if (!file) {
        printf("Cannot open file\n");
        return 0;
    }

    // BMP header is always 54 bytes, contains width/height info
    unsigned char header[54];
    fread(header, 1, 54, file);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];

    // BMP rows are padded to be a multiple of 4 bytes
    int row_padded = (width * 3 + 3) & (~3);

    unsigned char* data = (unsigned char*)malloc(row_padded * height);
    fread(data, 1, row_padded * height, file);
    fclose(file);

    // remove the padding so we get a clean RGB array
    unsigned char* cleanData = (unsigned char*)malloc(width * height * 3);

    for (int i = 0; i < height; i++) {
        memcpy(cleanData + i * width * 3,
            data + i * row_padded,
            width * 3);
    }

    free(data);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // BMP stores pixels as BGR so we use GL_BGR_EXT
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        width, height, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, cleanData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(cleanData);
    return texture;
}


// loads both textures at startup and prints whether they loaded correctly
void init() {

    myTexture1 = LoadTexture(cavePath);

    if (myTexture1 == 0)
        cout << "Error loading cave texture" << endl;
    else
        cout << "Cave texture loaded: " << myTexture1 << endl;

    myTexture2 = LoadTexture(rockPath);

    if (myTexture2 == 0)
        cout << "Error loading rock texture" << endl;
    else
        cout << "Rock texture loaded: " << myTexture2 << endl;
}


// draws the ocean background used in most scenes
// gradient goes from dark blue at the bottom to light blue at the top
void drawBackground() {

    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.08f, 0.35f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glColor3f(0.0f, 0.65f, 0.95f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // uneven polygon for the sea floor so it does not look like a flat line
    glBegin(GL_POLYGON);
    glColor3f(0.42f, 0.28f, 0.12f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glColor3f(0.55f, 0.42f, 0.25f);
    glVertex2f(1.0f, -0.72f);
    glVertex2f(0.7f, -0.65f);
    glVertex2f(0.35f, -0.72f);
    glVertex2f(0.0f, -0.62f);
    glVertex2f(-0.35f, -0.70f);
    glVertex2f(-0.7f, -0.64f);
    glVertex2f(-1.0f, -0.72f);
    glEnd();
}


// draws several seaweed plants along the sea floor
// each plant has a stem and small triangle leaves on alternating sides
void drawSeaweed() {

    float positions[] = { -0.85f, -0.55f, -0.25f, 0.35f, 0.65f, 0.85f };
    float scales[] = { 1.0f, 1.2f, 0.9f, 1.1f, 1.3f, 0.95f };

    for (int p = 0; p < 6; p++) {

        float x = positions[p];
        float baseY = -0.90f;
        float scale = scales[p];

        glColor3f(0.0f, 0.35f, 0.08f);
        glLineWidth(10.0f);
        glBegin(GL_LINES);
        glVertex2f(x, baseY);
        glVertex2f(x, baseY + 0.55f * scale);
        glEnd();

        for (int i = 0; i < 5; i++) {

            float y = baseY + 0.10f * scale + i * 0.10f * scale;
            float leafSize = (0.13f - i * 0.01f) * scale;

            glColor3f(0.0f, 0.45f + i * 0.04f, 0.12f);
            glBegin(GL_TRIANGLES);

            if (i % 2 == 0) {
                glVertex2f(x, y);
                glVertex2f(x, y + 0.07f * scale);
                glVertex2f(x + leafSize, y + 0.04f * scale);
            }
            else {
                glVertex2f(x, y);
                glVertex2f(x, y + 0.07f * scale);
                glVertex2f(x - leafSize, y + 0.04f * scale);
            }

            glEnd();
        }
    }
}


// dark background used only in the cave scene (scene 6)
void drawMyBackground() {
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.18f, 0.32f); glVertex2f(-1, 1);
    glColor3f(0.0f, 0.18f, 0.32f); glVertex2f(1, 1);
    glColor3f(0.0f, 0.0f, 0.0f);  glVertex2f(1, -1);
    glColor3f(0.0f, 0.0f, 0.0f);  glVertex2f(-1, -1);
    glEnd();
}


// draws a fish shape centered at the origin
// color is controlled by r, g, b so we can reuse this for all three family members
void drawFish(float r, float g, float b) {

    // body
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.15f * cos(angle), 0.08f * sin(angle));
    }
    glEnd();

    // tail
    glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.15f, 0.0f);
    glVertex2f(-0.22f, 0.08f);
    glVertex2f(-0.22f, -0.08f);
    glEnd();

    // top fin
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.06f, 0.07f);
    glVertex2f(0.00f, 0.16f);
    glVertex2f(0.06f, 0.07f);
    glEnd();

    // bottom fin
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.06f, -0.07f);
    glVertex2f(0.00f, -0.15f);
    glVertex2f(0.06f, -0.07f);
    glEnd();

    // white part of the eye
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.09f + 0.03f * cos(angle), 0.03f + 0.03f * sin(angle));
    }
    glEnd();

    // pupil
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.095f + 0.015f * cos(angle), 0.03f + 0.015f * sin(angle));
    }
    glEnd();

    // mouth
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.08f, -0.03f);
    glVertex2f(0.11f, -0.04f);
    glVertex2f(0.13f, -0.025f);
    glEnd();
    glLineWidth(1.0f);

    // scale lines on the body
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
    glBegin(GL_LINES);
    glVertex2f(-0.05f, 0.04f); glVertex2f(-0.02f, -0.04f);
    glVertex2f(0.00f, 0.05f); glVertex2f(0.03f, -0.05f);
    glVertex2f(0.05f, 0.04f); glVertex2f(0.08f, -0.04f);
    glEnd();
}

// draws the small glowing object on the sea floor that distracts the small fish
void drawShinyObject() {
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.05f * cos(angle), 0.05f * sin(angle));
    }
    glEnd();
}

// scene 1: the whole family swims together from left to right
void drawScene1() {

    drawBackground();
    drawSeaweed();

    glPushMatrix();
    glTranslatef(0.0f, -0.75f, 0.0f);
    drawShinyObject();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(smallFishX, smallFishY, 0.0f);
    glScalef(0.9f, 0.9f, 1.0f);
    drawFish(1.0f, 0.5f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(smallFishX + 0.35f, smallFishY + 0.18f, 0.0f);
    glScalef(1.2f, 1.2f, 1.0f);
    drawFish(1.0f, 0.3f, 0.6f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(smallFishX + 0.38f, smallFishY - 0.18f, 0.0f);
    glScalef(1.35f, 1.35f, 1.0f);
    drawFish(0.1f, 0.5f, 1.0f);
    glPopMatrix();
}


// scene 2: the small fish notices the shiny object and stops
// parents keep moving forward without noticing
void drawScene2() {

    drawBackground();
    drawSeaweed();

    glPushMatrix();
    glTranslatef(0.0f, -0.75f, 0.0f);
    drawShinyObject();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(smallFishX, smallFishY, 0.0f);
    glScalef(0.9f, 0.9f, 1.0f);
    drawFish(1.0f, 0.5f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(parentsX + 0.35f, 0.25f, 0.0f);
    glScalef(1.2f, 1.2f, 1.0f);
    drawFish(1.0f, 0.3f, 0.6f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(parentsX + 0.38f, -0.05f, 0.0f);
    glScalef(1.35f, 1.35f, 1.0f);
    drawFish(0.1f, 0.5f, 1.0f);
    glPopMatrix();
}


// scene 3: small fish dives down toward the object
// rotation makes her body tilt during the dive
void drawScene3() {

    drawBackground();
    drawSeaweed();

    glPushMatrix();
    glTranslatef(0.0f, -0.75f, 0.0f);
    drawShinyObject();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(smallFishX, smallFishY, 0.0f);

    if (smallFishY > -0.65f)
        glRotatef(-60.0f, 0.0f, 0.0f, 1.0f);
    else
        glRotatef(0.0f, 0.0f, 0.0f, 1.0f);

    glScalef(0.9f, 0.9f, 1.0f);
    drawFish(1.0f, 0.5f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(parentsX + 0.35f, 0.25f, 0.0f);
    glScalef(1.2f, 1.2f, 1.0f);
    drawFish(1.0f, 0.3f, 0.6f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(parentsX + 0.38f, -0.05f, 0.0f);
    glScalef(1.35f, 1.35f, 1.0f);
    drawFish(0.1f, 0.5f, 1.0f);
    glPopMatrix();
}


// scene 4: small fish comes back up and realizes her parents are gone
// three phases: rise, pause, then look left and right
void drawScene4() {

    drawBackground();
    drawSeaweed();

    glPushMatrix();
    glTranslatef(0.0f, -0.75f, 0.0f);
    drawShinyObject();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, smallFishY, 0.0f);

    if (scene4State == 0) {
        // still rising, body tilted
        glRotatef(80.0f, 0.0f, 0.0f, 1.0f);
        drawFish(1.0f, 0.6f, 0.2f);
    }
    else if (scene4State == 1) {
        // reached the top, standing straight
        glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
        drawFish(1.0f, 0.6f, 0.2f);
    }
    else {
        // looking left and right using sin to alternate the flip
        float t = sin(glutGet(GLUT_ELAPSED_TIME) * 0.002f);
        if (t > 0)
            glScalef(0.9f, 0.9f, 1.0f);
        else
            glScalef(-0.9f, 0.9f, 1.0f);
        drawFish(1.0f, 0.6f, 0.2f);
    }

    glPopMatrix();

    // parents are far off screen to the right by this point
    glPushMatrix();
    glTranslatef(parentsX + 2.5f, 0.25f, 0.0f);
    drawFish(1.0f, 0.3f, 0.6f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(parentsX + 2.6f, -0.05f, 0.0f);
    drawFish(0.1f, 0.5f, 1.0f);
    glPopMatrix();
}


// scene 5: small fish swims to the left searching for her parents
void drawScene5() {

    drawBackground();
    drawSeaweed();

    glPushMatrix();
    glTranslatef(0.0f, -0.75f, 0.0f);
    drawShinyObject();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(scene5FishX, smallFishY, 0.0f);
    glScalef(-0.9f, 0.9f, 1.0f); // facing left
    drawFish(1.0f, 0.6f, 0.2f);
    glPopMatrix();
}


// scene 6: small fish reaches the cave entrance
// uses the dark background, floor, cave walls, and floating bubbles
void drawScene6() {

    drawMyBackground();
    drawFloor();
    drawCave();
    drawBubbles();

    glPushMatrix();
    glTranslatef(fishX, -0.1f, 0.0f);
    glScalef(-1, 1, 1); // facing left toward the cave
    drawFish(1.0f, 0.5f, 0.0f);
    glPopMatrix();
}


// draws the cave texture as a full screen background for scene 7
void drawScene7Background() {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myTexture1);
    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1, 0); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1, 1); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0, 1); glVertex2f(-1.0f, 1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


// scene 7: the shark appears from the cave and the small fish escapes
// fish enters from the right, freezes when it sees the shark, then runs
void drawScene7() {

    drawScene7Background();

    glPushMatrix();
    glTranslatef(smallFishX, -0.1f, 0.0f);

    if (delay < 140)
        glScalef(-0.8f, 0.8f, 1.0f); // facing left (toward the shark)
    else
        glScalef(0.8f, 0.8f, 1.0f); // flipped right to escape

    drawFish(1.0f, 0.5f, 0.0f);
    glPopMatrix();

    // shark body
    glPushMatrix();
    glTranslatef(sharkX, -0.1f, 0.0f);

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.35f * cos(angle), 0.18f * sin(angle));
    }
    glEnd();

    // tail
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.35f, 0.0f);
    glVertex2f(-0.5f, 0.06f);
    glVertex2f(-0.5f, -0.06f);
    glEnd();

    // eye
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.2f + 0.03f * cos(angle), 0.06f + 0.03f * sin(angle));
    }
    glEnd();

    // pupil
    glColor3f(0, 0, 0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.2f + 0.015f * cos(angle), 0.06f + 0.015f * sin(angle));
    }
    glEnd();

    //  mouth

    glBegin(GL_LINE);
    glVertex2f(0.25f, -0.02f);
    glVertex2f(0.35f, -0.06f);
    glEnd();

    // teeth
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.27f, 0.02f);
    glVertex2f(0.30f, 0.02f);
    glVertex2f(0.285f, -0.01f);
    glVertex2f(0.30f, 0.02f);
    glVertex2f(0.33f, 0.02f);
    glVertex2f(0.319f, -0.01f);
    glEnd();

    glPopMatrix();
}


// scene 8: small fish returns, parents come back, family reunites
// ends with a text message on screen
void drawScene8() {

    // apply 2D viewing with zoom using glOrtho
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-zoomLevel, zoomLevel, -zoomLevel, zoomLevel, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawBackground();
    drawSeaweed();
    drawBubbles();

    // small fish
    glPushMatrix();
    glTranslatef(scene8FishX, scene8FishY, 0.0f);

    if (scene8State == 1) {
        float look = sin(scene8Timer * 0.08f);
        if (look > 0)
            glScalef(0.9f, 0.9f, 1.0f);
        else
            glScalef(-0.9f, 0.9f, 1.0f);
    }
    else {
        glScalef(0.9f, 0.9f, 1.0f);
    }

    drawFish(1.0f, 0.5f, 0.0f);
    glPopMatrix();

    // mother coming from the right
    glPushMatrix();
    glTranslatef(scene8ParentX, 0.15f, 0.0f);
    glScalef(-1.2f, 1.2f, 1.0f);
    drawFish(1.0f, 0.3f, 0.6f);
    glPopMatrix();

    // father coming from the right
    glPushMatrix();
    glTranslatef(scene8ParentX, -0.15f, 0.0f);
    glScalef(-1.35f, 1.35f, 1.0f);
    drawFish(0.1f, 0.5f, 1.0f);
    glPopMatrix();

    // show the final message once everyone has arrived
    if (scene8State == 3) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(-0.38f, 0.8f);
        string text = "Stay close to your family.";
        for (char c : text)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}


// flat dark floor drawn at the bottom of the cave scene
void drawFloor() {
    glBegin(GL_QUADS);
    glColor3f(0.17f, 0.13f, 0.07f); glVertex2f(-1, -0.62f);
    glColor3f(0.17f, 0.13f, 0.07f); glVertex2f(1, -0.62f);
    glColor3f(0.08f, 0.06f, 0.03f); glVertex2f(1, -1.0f);
    glColor3f(0.08f, 0.06f, 0.03f); glVertex2f(-1, -1.0f);
    glEnd();
}


// draws the cave walls, a dim light beam from above, and the rock texture on the floor
void drawCave() {

    // dark base fill
    glBegin(GL_QUADS);
    glColor3f(0.02f, 0.02f, 0.02f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // left wall with color gradient to fake depth
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(0.15f, 0.10f, 0.07f); glVertex2f(-1.0f, 1.0f);
    glColor3f(0.05f, 0.04f, 0.03f); glVertex2f(-0.4f, 0.6f);
    glColor3f(0.12f, 0.08f, 0.05f); glVertex2f(-1.0f, 0.0f);
    glColor3f(0.04f, 0.03f, 0.02f); glVertex2f(-0.3f, 0.0f);
    glColor3f(0.10f, 0.07f, 0.04f); glVertex2f(-1.0f, -1.0f);
    glColor3f(0.03f, 0.02f, 0.02f); glVertex2f(-0.5f, -0.8f);
    glEnd();

    // right wall
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(0.05f, 0.04f, 0.03f); glVertex2f(0.3f, 0.7f);
    glColor3f(0.12f, 0.08f, 0.05f); glVertex2f(1.0f, 1.0f);
    glColor3f(0.04f, 0.03f, 0.02f); glVertex2f(0.2f, 0.0f);
    glColor3f(0.10f, 0.07f, 0.04f); glVertex2f(1.0f, 0.0f);
    glColor3f(0.03f, 0.02f, 0.02f); glVertex2f(0.4f, -1.0f);
    glColor3f(0.08f, 0.05f, 0.03f); glVertex2f(1.0f, -1.0f);
    glEnd();

    // faint light beam coming from the top opening
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLES);
    glColor4f(0.6f, 0.7f, 0.8f, 0.25f); glVertex2f(-0.1f, 1.0f);
    glColor4f(0.05f, 0.05f, 0.07f, 0.0f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.3f, -0.5f);
    glEnd();

    glDisable(GL_BLEND);

    // rock texture on the cave floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myTexture2);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -0.8f);
    glTexCoord2f(3.0f, 0.0f); glVertex2f(1.0f, -0.8f);
    glTexCoord2f(3.0f, 1.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


// small circles that float upward, used in the cave and scene 8
void drawBubbles() {
    for (int i = 0; i < 6; i++) {
        glColor3f(0.8f, 0.9f, 1.0f);
        glBegin(GL_LINE_LOOP);
        for (int a = 0; a < 360; a += 20) {
            glVertex2f(
                -0.9f + i * 0.3f + 0.02f * cos(a * 3.14 / 180),
                bubY[i] + 0.02f * sin(a * 3.14 / 180)
            );
        }
        glEnd();
    }
}


// calls the right drawing function based on the current scene number
void display() {

    glClear(GL_COLOR_BUFFER_BIT);

    switch (currentScene) {
    case 1: drawScene1(); break;
    case 2: drawScene2(); break;
    case 3: drawScene3(); break;
    case 4: drawScene4(); break;
    case 5: drawScene5(); break;
    case 6: drawScene6(); break;
    case 7: drawScene7(); break;
    case 8: drawScene8(); break;
    default: drawScene1(); break;
    }

    glutSwapBuffers();
}


// updates all position variables and schedules the next frame
void timer(int value) {

    if (currentScene == 1) {
        smallFishX += 0.005f;
        if (smallFishX > 1.4f)
            smallFishX = -1.2f;
    }

    if (currentScene == 2) {
        parentsX += 0.005f;
        if (smallFishState == 0) {
            smallFishX += 0.005f;
            if (smallFishX > 0.0f)
                smallFishState = 1;
        }
        else if (smallFishState == 1) {
            if (smallFishY > -0.65f)
                smallFishY -= 0.005f;
            else
                smallFishState = 3;
        }
        else if (smallFishState == 2) {
            if (smallFishY > -0.7f)
                smallFishY -= 0.005f;
            else
                smallFishState = 3;
        }
    }

    if (currentScene == 3) {
        if (smallFishY > -0.65f)
            smallFishY -= 0.003f;
        parentsX += 0.005f;
    }

    if (currentScene == 4) {
        if (scene4State == 0) {
            if (smallFishY < 0.2f)
                smallFishY += 0.003f;
            else {
                smallFishY = 0.2f;
                scene4State = 1;
            }
        }
        else if (scene4State == 1) {
            smallFishY = 0.2f;
            if (glutGet(GLUT_ELAPSED_TIME) > 2000)
                scene4State = 2;
        }
        else if (scene4State == 2) {
            smallFishY = 0.2f;
            smallFishX = 0.0f;
        }
    }

    if (currentScene == 5) {
        scene5FishX -= 0.005f;
    }

    if (currentScene == 6) {
        fishX -= 0.004f;
        if (fishX < -1.5f)
            fishX = -1.5f;
    }

    if (currentScene == 7) {
        if (start) {
            sharkX = -1.0f;
            smallFishX = 1.2f;
            delay = 0;
            start = false;
        }
        delay++;
        if (delay < 90) {
            smallFishX -= 0.01f;
            sharkX += 0.005f;
        }
        else if (delay < 140) {
            // pause - fish is shocked
        }
        else {
            smallFishX += 0.02f;
            sharkX += 0.003f;
        }
        if (smallFishX > 1.5f)
            start = true;
    }

    if (currentScene == 8) {

        scene8Timer++;

        if (scene8State == 0) {
            scene8FishX += 0.018f;
            if (scene8FishX >= -0.25f) {
                scene8State = 1;
                scene8Timer = 0;
            }
        }
        else if (scene8State == 1) {
            scene8FishX = -0.25f + 0.18f * sin(scene8Timer * 0.08f);
            if (scene8Timer > 180)
                scene8State = 2;
        }
        else if (scene8State == 2) {
            if (scene8ParentX > 0.25f)
                scene8ParentX -= 0.006f;
            else
                scene8State = 3;
        }

        // zoom in slowly once everyone is together
        if (scene8State == 3 && zoomLevel > 0.5f) {
            zoomLevel -= 0.002f;
        }
    }

    // bubbles float up in all scenes
    for (int i = 0; i < 6; i++) {
        bubY[i] += 0.003f;
        if (bubY[i] > 1.1f)
            bubY[i] = -0.9f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}


// handles left and right arrow keys to move between scenes
void specialKeys(int key, int x, int y) {

    if (key == GLUT_KEY_RIGHT) {
        currentScene++;
        if (currentScene == 5) scene5FishX = 0.0f;
        if (currentScene > 8)  currentScene = 1;
    }

    if (key == GLUT_KEY_LEFT) {
        currentScene--;
        if (currentScene == 5) scene5FishX = 0.0f;
        if (currentScene < 1)  currentScene = 8;
    }

    // reset zoom and projection when leaving scene 8
    if (currentScene != 8) {
        zoomLevel = 1.0f;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    glutPostRedisplay();
}


int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("The Lost Fish");

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutSpecialFunc(specialKeys);

    init();

    glutMainLoop();
    return 0;
}
