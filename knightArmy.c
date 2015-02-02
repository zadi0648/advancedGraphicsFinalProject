/*
   Zack Diller
   CSCI 5229 FINAL PROJECT
   Files:
   > knightArmy.c
   spriteMaker.c
   knightDrawer.c
   knightDrawer.h

   File Description: This program does most of the actual work seen in the final product. NOTE: Must be run after spriteMaker is ran in order to work!

   Please see README.txt for more info
*/

#include "CSCIx239.h"
#include "knightDrawer.h"
#ifdef _WIN32
#include <GL/glew.h>
#else
#include <GL/glut.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL/SDL.h"

// ----- CAMERAS -----
static const float _cameraRotSpeed = 0.5; // Speed at which the camera rotates
int _selectedScreen = 0; // Whether the user is giving attention to the first or second window (0 = main, 1 = seconday, -1 = none)
int _showSplitScreen = 1; // Show split screen?
int _minPh = -89; // Min elevation
int _maxPh = +89; // Min elevation
float _splitScreenSeam = 0.5; // Location of split screen split
static const float _splitScreenSeamWidth = 0.01; // Area in which the cursor is considered over the seam
static const float _splitScreenSeamLimits[2] = {0.15, 0.85};
int _canDragSeam = 0; // If the user can drag the seam
int _draggingSeam = 0; // If the user is dragging the seam
float _asp = 1.0; // Aspect ratio
float _aspectSaved;
int _ignoreJump = 1; // Should we ignore mouse loc this frame?
// ----- Camera 1 ----- 
float _th[2]; // Azimuth of view angle
float _ph[2]; // Elevation of view angle
float _cameraLoc[3]; // Camera location
float _cameraLook1[3]; // Location that camera is pointed at
float _cursorSpeed = 0.3; // Rotation due to mouse
static const float _travelSpeed[2] = {0.2, 0.8}; // Speed of x&z translation
static const float _elevationSpeed[2] = {0.2, 0.8}; // Speed of y translation
int _fov = 53; // Field of view
float _worldSize = 30.0; // Size of world
float _modelMatrix[16]; // Model view matrix - prior to terrian adjusting
float _projMatrix[16]; // Projection matrix - prior to terrian adjusting
float _mvpAdjusted[16]; // MVP matrix - adjusted for terrian
// ----- Camera 2 ----- 
static const float _cameraLook2[3] = {0.0,10.2,0.0}; // Location of our 'general knight'
float _camera2Dist = 1.0; // Distance away from point
static const float _camera2DistLimits[2] = {0.2, 3.0};
static const float _camera2DistSpeed = 0.02;
float _modelMatrix2[16]; // Model view matrix 2 - prior to terrian adjusting
float _projMatrix2[16]; // Projection matrix 2 - prior to terrian adjusting
float _mvpAdjusted2[16]; // MVP matrix 2 - adjusted for terrian
// ----- Environment ----- 
#define mapSize 1024
#define gridSize 64
float _tVerts[(gridSize+1)*(gridSize+1)*3];
int _tFaces[(gridSize+1)*(gridSize)*4];
GLuint _environmentShad0;
GLuint _environmentShad1;
GLuint _environmentShad2;
GLuint _environmentShad3;
int _skyBoxShader; // Skybox shader
int _terrianMode = 0; // Which shader program we are currently using
static const float _skyBoxSize = 215.0; // Size of sky box
static const float _skyHeight = 40.0; // Height of sky box
static const float _landSize = 430.0; // Width of environment
static const float _mountainHeights = 7.5;
int _Nterr; // Number of patches to draw
int _Positions; //  _Positions VBO
// ----- Lighting -----
static const float _lightHeight = 0.7;
float _lightAngle;
const const float _defaultLightAngle = 10.0;
float _lightPosition[4];
float _lightAngleSpeed = 1.0;
static const float _lightEmission[] = {0.3,0.3,0.3,1.0};
static const float _lightAmbient[] = {0.1,0.1,0.1,1.0};
static const float _lightDiffuse[] = {1.0,1.0,1.0,1.0};
static const float _lightSpecular[] = {1.0,1.0,1.0,1.0};
static const float _lightShinyness[] = {16};
// ----- Level of Detail -----
// Terrian/environment
int _tForcedLOD = 0; // Whether to use camera LOD or have the user specify LOD for terrian
int _tLOD = 1; // Level of Detail of terrian
static const int _maxTLOD = 10; // Max of terrian LOD
// Knights
int _kForcedLOD = 0; // Whether to use camera LOD or have the user specify LOD for knights
int _kLOD = 1; // Level of Detail of knights
static const float _particleThreshold = 50.0;
// ----- Keyboard & mouse ----- 
int _keyPressed[24]; // Key press variables
// --------------------------------
// (0-3) W,A,S,D = Movement
// (4-5) Q&E = Alter camera elevation
// (6) Shift = Speed up
// (7) Z = Free cursor
// (8) X = Switch cameras, if splitscreen
// (9) C = Disable splitscreen
// (10) T = Switch terrian mode
// (11) G = Forced terrian LOD
// (12-13) Y&H = Increase/decrease LOD when in forced mode
// (14-15) V&B = Change general knight rotation (to show off normal-mapping shader)
// (16-17) R&F = Change sun location
// (18) U = Show normal-mapping test object
// (19) O = Switch which unit you have control of
// (20-23) I/J/K/L = Move unit of knights
// --------------------------------
// ----- Application variables ----- 
SDL_Surface *_window; // the window pointer
int _windowW = 0.0;
int _windowH = 0.0;
char _title[250]; // Title of our application
// ----- World & game variables ----- 
unsigned int _running = 1; // Equals one as long as the program is running
unsigned int _textures[11]; // Non-generated textures this program uses
// ----- FPS -----
float _fps = 0.0;
unsigned int _lastFrame;
unsigned int _thisFrame;
unsigned int _frameCount = 0;
unsigned int _tickCount = 0;
// ----- Knight Variables -----
unsigned int _knightParticleShader;
#define knightCount 101 // 1 general + 2 units of 50
float _mainKnightRot = 0;
static const float _mainKnightRotSpeed = 1.5;
#define particleImgCount 1368
static const float _knightScale = 0.55;
unsigned int _knightParticleImgs[particleImgCount]; // # subject to change!
float _knightPositions[knightCount * 3];
unsigned int _thPerPic; // How much _th needs to change before we use a different particle
unsigned int _phPerPic; // How much _ph needs to change before we use a different particle
unsigned int _thUse = 0; // Current th amount being used
unsigned int _phUse = 0; // Current ph amount being used
unsigned int _particleUse; // Which image to use for the particle
unsigned int _phPerTh; // Use to determine which image we should use
unsigned int _particleCount = knightCount; // Number of knight particles to be drawn (initailize to max for clean-out)
unsigned int _particleArrangedOrderIndex[knightCount]; // Index of knight to be draw
float _particleArrangedOrderDistance[knightCount]; // Distance of the above index
float _knightScaledHeight; // Used in shaders to determine the height translation to match terrian
unsigned int _controledUnit = 0; // Which unit you have control of
float _unit1translation[2]; // Translation of unit 1
float _unit2translation[2]; // Translation of unit 2
static const float _translationSpeed = 0.4; // How fast you can translate a unit
static const float _translationMax[] = {70.0, 70.0}; // How far you can translate a unit
// ----- Cursor images -----
SDL_Cursor *_cursorNone;
SDL_Cursor *_cursorArrow;
SDL_Cursor *_cursorPreDrag;
SDL_Cursor *_cursorDragging;
static const char *_arrowCursor[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "X                               ",
  "XX                              ",
  "X.X                             ",
  "X..X                            ",
  "X...X                           ",
  "X....X                          ",
  "X..X..X                         ",
  "X..XX..X                        ",
  "X..X.X..X                       ",
  "X..X..X..X                      ",
  "X..X...X..X                     ",
  "X..X....X..X                    ",
  "X..X.....X..X                   ",
  "X..X......X..X                  ",
  "X..X.......X..X                 ",
  "X..X...XXXXXX..X                ",
  "X..X..X.........X               ",
  "X..X.X..XXXXXXXXXX              ",
  "X..XX..X                        ",
  "X..X..X                         ",
  "X....X                          ",
  "X...X                           ",
  "X..X                            ",
  "X.X                             ",
  "XX                              ",
  "X                               ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,0"
};
static const char *_noneCursor[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,0"
};
static const char *_preDragCursor[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "     X                    X     ",
  "    X.X                  X.X    ",
  "   X..X                  X..X   ",
  "  X...X                  X...X  ",
  " X....XXXXXXXXXXXXXXXXXXXX....X ",
  "X..............................X",
  " X....XXXXXXXXXXXXXXXXXXXX....X ",
  "  X...X                  X...X  ",
  "   X..X                  X..X   ",
  "    X.X                  X.X    ",
  "     X                    X     ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "16,16"
};
static const char *_draggingCursor[] = {
  /* width height num_colors chars_per_piXel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* piXels */
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "     X                    X     ",
  "    XXX                  XXX    ",
  "   XXXX                  XXXX   ",
  "  XXXXX                  XXXXX  ",
  " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ",
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ",
  "  XXXXX                  XXXXX  ",
  "   XXXX                  XXXX   ",
  "    XXX                  XXX    ",
  "     X                    X     ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "16,16"
};
// Misc.
unsigned int _showTest = 0;

// Help from: https://wiki.libsdl.org/SDL_CreateCursor
static SDL_Cursor *init_system_cursor(const char *image[], int hX, int hY)
{
   int i, row, col;
   Uint8 data[4*32];
   Uint8 mask[4*32];

   i = -1;
   for (row=0; row<32; ++row) {
      for (col=0; col<32; ++col) {
         if (col % 8) {
            data[i] <<= 1;
            mask[i] <<= 1;
         } else {
            ++i;
            data[i] = mask[i] = 0;
         }
      switch (image[4+row][col]) {
         case 'X':
            data[i] |= 0x01;
            mask[i] |= 0x01;
            break;
         case '.':
            mask[i] |= 0x01;
            break;
         case ' ':
            break;
         }
      }
   }
   return SDL_CreateCursor(data, mask, 32, 32, hX, hY);
}

// Matrix multiplication of two 16-float arrays the represent 4x4 matricies
void matrixDotProduct16x16(float *sol, float *m1, float *m2)
{
   sol[0]  = (m1[0]  * m2[0]) + (m1[1]  * m2[4]) + (m1[2]  * m2[8])  + (m1[3]  * m2[12]);
   sol[1]  = (m1[0]  * m2[1]) + (m1[1]  * m2[5]) + (m1[2]  * m2[9])  + (m1[3]  * m2[13]);
   sol[2]  = (m1[0]  * m2[2]) + (m1[1]  * m2[6]) + (m1[2]  * m2[10]) + (m1[3]  * m2[14]);
   sol[3]  = (m1[0]  * m2[3]) + (m1[1]  * m2[7]) + (m1[2]  * m2[11]) + (m1[3]  * m2[15]);

   sol[4]  = (m1[4]  * m2[0]) + (m1[5]  * m2[4]) + (m1[6]  * m2[8])  + (m1[7]  * m2[12]);
   sol[5]  = (m1[4]  * m2[1]) + (m1[5]  * m2[5]) + (m1[6]  * m2[9])  + (m1[7]  * m2[13]);
   sol[6]  = (m1[4]  * m2[2]) + (m1[5]  * m2[6]) + (m1[6]  * m2[10]) + (m1[7]  * m2[14]);
   sol[7]  = (m1[4]  * m2[3]) + (m1[5]  * m2[7]) + (m1[6]  * m2[11]) + (m1[7]  * m2[15]);

   sol[8]  = (m1[8]  * m2[0]) + (m1[9]  * m2[4]) + (m1[10] * m2[8])  + (m1[11] * m2[12]);
   sol[9]  = (m1[8]  * m2[1]) + (m1[9]  * m2[5]) + (m1[10] * m2[9])  + (m1[11] * m2[13]);
   sol[10] = (m1[8]  * m2[2]) + (m1[9]  * m2[6]) + (m1[10] * m2[10]) + (m1[11] * m2[14]);
   sol[11] = (m1[8]  * m2[3]) + (m1[9]  * m2[7]) + (m1[10] * m2[11]) + (m1[11] * m2[15]);

   sol[12] = (m1[12] * m2[0]) + (m1[13] * m2[4]) + (m1[14] * m2[8])  + (m1[15] * m2[12]);
   sol[13] = (m1[12] * m2[1]) + (m1[13] * m2[5]) + (m1[14] * m2[9])  + (m1[15] * m2[13]);
   sol[14] = (m1[12] * m2[2]) + (m1[13] * m2[6]) + (m1[14] * m2[10]) + (m1[15] * m2[14]);
   sol[15] = (m1[12] * m2[3]) + (m1[13] * m2[7]) + (m1[14] * m2[11]) + (m1[15] * m2[15]);
}
// No longer used, but kept anyhow because I spent time writing it
void matrixDotProduct16x4(float *sol, float *m1, float *m2)
{
   sol[0]  = (m2[0]  * m1[0]) +  (m2[1]  * m1[1]) +  (m2[2]  * m1[2])  + (m2[3]  * m1[3]);
   sol[1]  = (m2[0]  * m1[4]) +  (m2[1]  * m1[5]) +  (m2[2]  * m1[6])  + (m2[3]  * m1[7]);
   sol[2]  = (m2[0]  * m1[8]) +  (m2[1]  * m1[9]) +  (m2[2]  * m1[10]) + (m2[3]  * m1[11]);
   sol[3]  = (m2[0]  * m1[12]) + (m2[1]  * m1[13]) + (m2[2]  * m1[14]) + (m2[3]  * m1[15]);
}
// Not used, was used for debugging purposes
void printMatrix(float *p) {
   printf("[%.1f,%.1f,%.1f,%.1f]\n[%.1f,%.1f,%.1f,%.1f]\n[%.1f,%.1f,%.1f,%.1f]\n[%.1f,%.1f,%.1f,%.1f]\n",
      p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
}

// 3D distance formula
float getDist(float x1, float y1, float z1, float x2, float y2, float z2) {
   float d;
   d =
      ((x1-x2)*(x1-x2)) +
      ((y1-y2)*(y1-y2)) +
      ((z1-z2)*(z1-z2));
   d = sqrt(d);
   return d;
}

// Selects which particle we should use for the knight particle shader
void selectParticleImg(float th, float ph) {
   unsigned int newUseTh;
   unsigned int newUsePh;
   unsigned int resTh;
   unsigned int resPh;
   float moduTh;
   float moduPh;
   float thPerPicF = (float)_thPerPic;
   float phPerPicF = (float)_phPerPic;
   // Limit th & ph to positive numbers
   if (th < 0) th += 360;
   if (ph >= 0) ph = 0;
   else ph = -ph;
   // Get divisor
   resTh = ((unsigned int)th) / _thPerPic;
   resPh = ((unsigned int)ph) / _phPerPic;
   // Get remainder
   moduTh = fmod(th, thPerPicF);
   moduPh = fmod(ph, phPerPicF);
   // Round th & ph appropraitely
   if (moduTh > (thPerPicF/2.0)) resTh++;
   if (moduPh > (phPerPicF/2.0)) resPh++;
   // New values found
   newUseTh = resTh * _thPerPic;
   newUsePh = resPh * _phPerPic;
   if (newUseTh >= 360) newUseTh = 0;

   // Do we need to change the image being used?
   if ((_thUse != newUseTh) || (_phUse != newUsePh)) {
      _thUse = newUseTh;
      _phUse = newUsePh;
      _particleUse = (newUseTh/_thPerPic) + ((newUsePh/_phPerPic)*_phPerTh);
      glActiveTexture(GL_TEXTURE9);
      glBindTexture(GL_TEXTURE_2D, _knightParticleImgs[_particleUse]);
   }
}

// For setting the light defaults
void setLightDefault()
{
   _lightAngle = _defaultLightAngle;
   _lightPosition[0] = Sin(_lightAngle);
   _lightPosition[1] = _lightHeight;
   _lightPosition[2] = Cos(_lightAngle);
}

// Set default angles & location for cameras (and other variables)
void setOrientationDefault()
{
   // Main camera
   _th[0] = -122.0;
   _ph[0] = -13.5;
   _cameraLoc[0] = 45.4;
   _cameraLoc[1] = 20.2;
   _cameraLoc[2] = -18.8;
   // Secondary camera
   _th[1] = -36.5;
   _ph[1] = +20.0;
   _mainKnightRot = 0.0;
   // Unit translation
   _unit1translation[0] = 30.0;
   _unit1translation[1] = -30.0;
   _unit2translation[0] = -30.0;
   _unit2translation[1] = 60.0;
   // Sun location
   setLightDefault();
}

// Inline functions for settings the matricies to what we want
inline void setMVP1Adj() {
   glGetFloatv(GL_MODELVIEW_MATRIX, _modelMatrix);
   glGetFloatv(GL_PROJECTION_MATRIX, _projMatrix);
   matrixDotProduct16x16(_mvpAdjusted, _modelMatrix, _projMatrix);
}
inline void setMVP2Adj() {
   glGetFloatv(GL_MODELVIEW_MATRIX, _modelMatrix);
   glGetFloatv(GL_PROJECTION_MATRIX, _projMatrix);
   matrixDotProduct16x16(_mvpAdjusted2, _modelMatrix, _projMatrix);
}

// Set up our adjusted mvp matrices
void setMVPadjusted(const int cam) {
   // Main MVP
   matrixDotProduct16x16(_mvpAdjusted, _modelMatrix, _projMatrix); // Matrix multiplication
   // Secondary MVP
   if (cam == 1) {
      matrixDotProduct16x16(_mvpAdjusted2, _modelMatrix2, _projMatrix2); // Matrix multiplication
   }
   // Main camera can just use the same matrix for both
   else {
      _mvpAdjusted2[0] = _mvpAdjusted[0];
      _mvpAdjusted2[1] = _mvpAdjusted[1];
      _mvpAdjusted2[2] = _mvpAdjusted[2];
      _mvpAdjusted2[3] = _mvpAdjusted[3];
      _mvpAdjusted2[4] = _mvpAdjusted[4];
      _mvpAdjusted2[5] = _mvpAdjusted[5];
      _mvpAdjusted2[6] = _mvpAdjusted[6];
      _mvpAdjusted2[7] = _mvpAdjusted[7];
      _mvpAdjusted2[8] = _mvpAdjusted[8];
      _mvpAdjusted2[9] = _mvpAdjusted[9];
      _mvpAdjusted2[10] = _mvpAdjusted[10];
      _mvpAdjusted2[11] = _mvpAdjusted[11];
      _mvpAdjusted2[12] = _mvpAdjusted[12];
      _mvpAdjusted2[13] = _mvpAdjusted[13];
      _mvpAdjusted2[14] = _mvpAdjusted[14];
      _mvpAdjusted2[15] = _mvpAdjusted[15];
   }
}

// Set caption for program
void setCaption()
{
   unsigned int mode = _terrianMode + (_tForcedLOD + _tForcedLOD);

   // Natural - dependent
   if (mode == 0) {
      snprintf(_title, 250, "FPS(%.2f) Terrian Mode(Camera location dependent - natural) Sun Angle(%.1f) Selected Unit(%d)",
         _fps, _lightAngle, _controledUnit);
   }
   // Triangles - dependent
   else if (mode == 1) {
      snprintf(_title, 250, "FPS(%.2f) Terrian Mode(Camera location dependent - triangles) Sun Angle(%.1f) Selected Unit(%d)",
         _fps, _lightAngle, _controledUnit);
   }
   // Natural - dependent
   else if (mode == 2) {
      snprintf(_title, 250, "FPS(%.2f) Terrian Mode(User specified Level of Detail - natural) LOD(%d) Sun Angle(%.1f) Selected Unit(%d)",
         _fps, _tLOD, _lightAngle, _controledUnit);
   }
   // Triangles - dependent
   else {
      snprintf(_title, 250, "FPS(%.2f) Terrian Mode(User specified Level of Detail - triangles) LOD(%d) Sun Angle(%.1f) Selected Unit(%d)",
         _fps, _tLOD, _lightAngle, _controledUnit);
   }

   SDL_WM_SetCaption(_title, 0);
}


// Draw a knight as a billboarding sprite
// cam == 0: splitscreen, main camera
// cam == 1: splitscreen, secondary camera
// cam == 2: no splitscreen (main camera only)
void drawKnightPoints(const int cam) {
   // For testing...
   /*
   if (_frameCount == 50) {
      printf("START\n");
      int y;
      for (y = 0; y < _particleCount; y++) {
         printf("[I:%d][Dist:%.2f](%.2f,%.2f,%.2f)\n", _particleArrangedOrderIndex[y], _particleArrangedOrderDistance[y],
            _knightPositions[(_particleArrangedOrderIndex[y]*3)+0],
            _knightPositions[(_particleArrangedOrderIndex[y]*3)+1],
            _knightPositions[(_particleArrangedOrderIndex[y]*3)+2]);
      }
      printf("END\n");
   }
   */
   int id;

   // Use our particle shader
   glUseProgram(_knightParticleShader);
   // Pass along shader variables
   id = glGetUniformLocation(_knightParticleShader,"modelViewProjection");
   if (cam == 1) {
      setMVP1Adj();
      if (id>=0) glUniformMatrix4fv(id,1,0,_mvpAdjusted);
   }
   else {
      setMVP2Adj();
      if (id>=0) glUniformMatrix4fv(id,1,0,_mvpAdjusted2);
   }
   id = glGetUniformLocation(_knightParticleShader,"terrain");
   if (id>=0) glUniform1i(id,1);
   // Adjust for environment variables
   id = glGetUniformLocation(_knightParticleShader,"landSize");
   if (id>=0) glUniform1f(id, _landSize);
   id = glGetUniformLocation(_knightParticleShader,"aspect");
   if (id>=0) glUniform1f(id, _aspectSaved);
   id = glGetUniformLocation(_knightParticleShader,"heightAdjust");
   if (id>=0) glUniform1f(id, (_landSize/_mountainHeights));
   id = glGetUniformLocation(_knightParticleShader,"knightSprite");
   if (id>=0) glUniform1i(id,9);
   // Enable blending & nearest pixel selecting
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   // Draw points in order of distance
   unsigned int partCount = _particleCount;
   int y;
   unsigned int knightIndex;
   // Draw the point
   for (y = 0; y < partCount; y++) {
      // Get this knight's index
      knightIndex = _particleArrangedOrderIndex[y];
      glBegin(GL_POINTS);

      if (knightIndex == 0) {
         glVertex3f(_knightPositions[0], _knightPositions[1], _knightPositions[2]);
      }
      else if ((knightIndex-1) < (knightCount-1)/2)
         glVertex3f(
            _knightPositions[(knightIndex*3)+0] + _unit1translation[0],
            _knightPositions[(knightIndex*3)+1],
            _knightPositions[(knightIndex*3)+2] + _unit1translation[1]);
      else
         glVertex3f(
            _knightPositions[(knightIndex*3)+0] + _unit2translation[0],
            _knightPositions[(knightIndex*3)+1],
            _knightPositions[(knightIndex*3)+2] + _unit2translation[1]);

      glEnd();
   }
   // Done
   glDisable(GL_BLEND);
   glUseProgram(0);
   ErrCheck("drawKnightPoints");
}

void cleanArray() {
   //unsigned int parCount = _particleCount;
   unsigned int t;
   // Set each distance to something huge
   for (t = 0; t < knightCount; t++) {
      _particleArrangedOrderDistance[t] = -1.0;
   }
   // Reset counter
   _particleCount = 0;
}

void putInArray(float dist, unsigned int index) {
   // Increment particle count
   unsigned int parCount = ++_particleCount;
   // Variables for this algorithm
   int spotFound = 0;
   unsigned int tempIndex;
   float tempDist;
   unsigned int y;
   // Find the correct place to put it in
   for (y = 0; y < parCount; y++) {
      // Have we found our spot? (will be when given distance is less than current)
      if (!spotFound) {
         if (dist > _particleArrangedOrderDistance[y]) {
            // We have found out spot
            spotFound = 1;
            // Prepare the array down
            tempIndex = _particleArrangedOrderIndex[y];
            tempDist = _particleArrangedOrderDistance[y];
            // Save our spot
            _particleArrangedOrderIndex[y] = index;
            _particleArrangedOrderDistance[y] = dist;
         }
      }
      // We have found our spot - move index down
      else {
         // Store
         unsigned int tI = _particleArrangedOrderIndex[y];
         float tD = _particleArrangedOrderDistance[y];
         _particleArrangedOrderIndex[y] = tempIndex;
         _particleArrangedOrderDistance[y] = tempDist;
         // Place next value
         tempIndex = tI;
         tempDist = tD;
      }
   }
}

void drawKnight(float transX, float transY, float transZ, float rot, int knightIndex) {
   // Get the distance from the knight and the main camera
   float dist = getDist(transX, transY, transZ, _cameraLoc[0], _cameraLoc[1], _cameraLoc[2]);

   // Draw full model
   if (dist < _particleThreshold) {
      drawKnightModel(transX,transY,transZ,rot,_knightScaledHeight,_landSize,_knightScale);
   }
   // Store as point to be drawn
   else putInArray(dist, knightIndex);
   
   ErrCheck("drawKnight");
}

// Draw the sky box
void drawSkyBox() {
   int id;

   float error = 0.25; // Accounts for floating-point precision error to fix skybox seams

   // Use our sky box shader
   glUseProgram(_skyBoxShader);
   // Pass along correct image to use (image #0)
   id = glGetUniformLocation(_skyBoxShader, "skyTexture");
   if (id>=0) glUniform1i(id,0);
   id = glGetUniformLocation(_skyBoxShader, "sunAngle");
   if (id>=0) glUniform1f(id,_lightAngle);

   glBegin(GL_QUADS);
   // Side 1
   glTexCoord2f(0.00, 0.0); glVertex3f(-_skyBoxSize, -_skyBoxSize + _skyHeight, -_skyBoxSize);
   glTexCoord2f(0.25, 0.0); glVertex3f(+_skyBoxSize, -_skyBoxSize + _skyHeight, -_skyBoxSize);
   glTexCoord2f(0.25, 0.5); glVertex3f(+_skyBoxSize, +_skyBoxSize + _skyHeight, -_skyBoxSize);
   glTexCoord2f(0.00, 0.5); glVertex3f(-_skyBoxSize, +_skyBoxSize + _skyHeight, -_skyBoxSize);
   // Side 2
   glTexCoord2f(0.25, 0.0); glVertex3f(+_skyBoxSize, -_skyBoxSize + _skyHeight, -_skyBoxSize);
   glTexCoord2f(0.50, 0.0); glVertex3f(+_skyBoxSize, -_skyBoxSize + _skyHeight, +_skyBoxSize);
   glTexCoord2f(0.50, 0.5); glVertex3f(+_skyBoxSize, +_skyBoxSize + _skyHeight, +_skyBoxSize);
   glTexCoord2f(0.25, 0.5); glVertex3f(+_skyBoxSize, +_skyBoxSize + _skyHeight, -_skyBoxSize);
   // Side 3
   glTexCoord2f(0.50, 0.0); glVertex3f(+_skyBoxSize, -_skyBoxSize + _skyHeight, +_skyBoxSize);
   glTexCoord2f(0.75, 0.0); glVertex3f(-_skyBoxSize, -_skyBoxSize + _skyHeight, +_skyBoxSize);
   glTexCoord2f(0.75, 0.5); glVertex3f(-_skyBoxSize, +_skyBoxSize + _skyHeight, +_skyBoxSize);
   glTexCoord2f(0.50, 0.5); glVertex3f(+_skyBoxSize, +_skyBoxSize + _skyHeight, +_skyBoxSize);
   // Side 4
   glTexCoord2f(0.75, 0.0); glVertex3f(-_skyBoxSize, -_skyBoxSize + _skyHeight, +_skyBoxSize);
   glTexCoord2f(1.00, 0.0); glVertex3f(-_skyBoxSize, -_skyBoxSize + _skyHeight, -_skyBoxSize);
   glTexCoord2f(1.00, 0.5); glVertex3f(-_skyBoxSize, +_skyBoxSize + _skyHeight, -_skyBoxSize);
   glTexCoord2f(0.75, 0.5); glVertex3f(-_skyBoxSize, +_skyBoxSize + _skyHeight, +_skyBoxSize);
   // Top
   glTexCoord2f(0.25, 0.5); glVertex3f(+(_skyBoxSize + error), +_skyBoxSize + _skyHeight - error, -(_skyBoxSize + error));
   glTexCoord2f(0.50, 0.5); glVertex3f(+(_skyBoxSize + error), +_skyBoxSize + _skyHeight - error, +(_skyBoxSize + error));
   glTexCoord2f(0.50, 1.0); glVertex3f(-(_skyBoxSize + error), +_skyBoxSize + _skyHeight - error, +(_skyBoxSize + error));
   glTexCoord2f(0.25, 1.0); glVertex3f(-(_skyBoxSize + error), +_skyBoxSize + _skyHeight - error, -(_skyBoxSize + error));
   glEnd();

   glUseProgram(0);

   ErrCheck("drawSkyBox");
}

// Draw the environment
// cam == 0: splitscreen, main camera
// cam == 1: splitscreen, secondary camera
// cam == 2: no splitscreen (main camera only)
void drawLandscape(const int cam)
{
   int id;

   // Scale
   float scale[16] =
   {
    _landSize,   0.0,    0.0, 0.0,
        0.0, _landSize,    0.0, 0.0,
        0.0,   0.0, -_landSize, 0.0,
        0.0,   0.0,    0.0, 1.0,
   };
   float mvpScaledMain[16]; // MVP1 after scale
   float mvpScaledSecondary[16]; // MVP2 after scale

   matrixDotProduct16x16(mvpScaledMain, scale, _mvpAdjusted);
   matrixDotProduct16x16(mvpScaledSecondary, scale, _mvpAdjusted2);

   // Main camera screen size
   float camSize[2];
   if (cam != 2) camSize[0] = (((float)_windowW)*_splitScreenSeam);
   camSize[0] = (float)_windowW;
   camSize[1] = (float)_windowH;

   // Set shader
   unsigned int mode = _terrianMode + (_tForcedLOD + _tForcedLOD);

   // Which shader program are we using on the environment?
   // I have to use conditional statements as opposed to an array
   // of ints representing the programs because I tended to run into
   // invalid value errors when I did use an array.
   // 0: Quads with tessellation dependent on camera
   if (mode == 0) {
      glUseProgram(_environmentShad0);
      // Give shader variable values
      id = glGetUniformLocation(_environmentShad0,"heightFactor");
      if (id>=0) glUniform1f(id,_mountainHeights);
      id = glGetUniformLocation(_environmentShad0,"lightDirection");
      if (id>=0) glUniform3f(id,_lightPosition[0],_lightPosition[1],_lightPosition[2]);
      id = glGetUniformLocation(_environmentShad0,"modelViewProjectionMain");
      if (id>=0) glUniformMatrix4fv(id,1,0,mvpScaledMain);
      id = glGetUniformLocation(_environmentShad0,"modelViewProjectionSecond");
      if (id>=0) glUniformMatrix4fv(id,1,0,mvpScaledSecondary);
      id = glGetUniformLocation(_environmentShad0,"sizeOfScreen");
      if (id>=0) glUniform2f(id,camSize[0]/2,camSize[1]/2);
      id = glGetUniformLocation(_environmentShad0,"dPerPix");
      if (id>=0) glUniform1f(id,1.0/1024.0);
      id = glGetUniformLocation(_environmentShad0,"terrain");
      if (id>=0) glUniform1i(id,1);
      id = glGetUniformLocation(_environmentShad0,"grass");
      if (id>=0) glUniform1i(id,2);
      id = glGetUniformLocation(_environmentShad0,"rock");
      if (id>=0) glUniform1i(id,3);
      id = glGetUniformLocation(_environmentShad0,"road");
      if (id>=0) glUniform1i(id,4);
   }
   // 1: Quads with tessellation dependent on camera
   else if (mode == 1) {
      glUseProgram(_environmentShad1);
      // Give shader variable values
      id = glGetUniformLocation(_environmentShad1,"heightFactor");
      if (id>=0) glUniform1f(id,_mountainHeights);
      id = glGetUniformLocation(_environmentShad1,"lightDirection");
      if (id>=0) glUniform3f(id,_lightPosition[0],_lightPosition[1],_lightPosition[2]);
      id = glGetUniformLocation(_environmentShad1,"modelViewProjectionMain");
      if (id>=0) glUniformMatrix4fv(id,1,0,mvpScaledMain);
      id = glGetUniformLocation(_environmentShad1,"modelViewProjectionSecond");
      if (id>=0) glUniformMatrix4fv(id,1,0,mvpScaledSecondary);
      id = glGetUniformLocation(_environmentShad1,"sizeOfScreen");
      if (id>=0) glUniform2f(id,camSize[0]/2,camSize[1]/2);
      id = glGetUniformLocation(_environmentShad1,"terrain");
      if (id>=0) glUniform1i(id,1);
   }
   // 2: Terrian with user-set level of detail
   else if (mode == 2) {
      glUseProgram(_environmentShad2);
      // Give shader variable values
      id = glGetUniformLocation(_environmentShad2,"LOD");
      if (id>=0) glUniform1f(id,_tLOD);
      id = glGetUniformLocation(_environmentShad2,"heightFactor");
      if (id>=0) glUniform1f(id,_mountainHeights);
      id = glGetUniformLocation(_environmentShad2,"lightDirection");
      if (id>=0) glUniform3f(id,_lightPosition[0],_lightPosition[1],_lightPosition[2]);
      id = glGetUniformLocation(_environmentShad2,"modelViewProjectionSecond");
      if (id>=0) glUniformMatrix4fv(id,1,0,mvpScaledSecondary);
      id = glGetUniformLocation(_environmentShad2,"dPerPix");
      if (id>=0) glUniform1f(id,1.0/1024.0);
      id = glGetUniformLocation(_environmentShad2,"terrain");
      if (id>=0) glUniform1i(id,1);
      id = glGetUniformLocation(_environmentShad2,"grass");
      if (id>=0) glUniform1i(id,2);
      id = glGetUniformLocation(_environmentShad2,"rock");
      if (id>=0) glUniform1i(id,3);
      id = glGetUniformLocation(_environmentShad2,"road");
      if (id>=0) glUniform1i(id,4);
   }
   // 3: Quads with user-set level of detail
   else {
      glUseProgram(_environmentShad3);
      // Give shader variable values
      id = glGetUniformLocation(_environmentShad3,"LOD");
      if (id>=0) glUniform1f(id,_tLOD);
      id = glGetUniformLocation(_environmentShad3,"heightFactor");
      if (id>=0) glUniform1f(id,_mountainHeights);
      id = glGetUniformLocation(_environmentShad3,"lightDirection");
      if (id>=0) glUniform3f(id,_lightPosition[0],_lightPosition[1],_lightPosition[2]);
      id = glGetUniformLocation(_environmentShad3,"modelViewProjectionSecond");
      if (id>=0) glUniformMatrix4fv(id,1,0,mvpScaledSecondary);
      id = glGetUniformLocation(_environmentShad3,"terrain");
      if (id>=0) glUniform1i(id,1);
   }

   // Draw the patches
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glClearColor(0.2,0.2,0.2,1.0);

   // Finally, draw the patches which will become the environment!
   glPatchParameteri(GL_PATCH_VERTICES,4);
   glDrawElements(GL_PATCHES,_Nterr,GL_UNSIGNED_INT,0);

   // Unset shader
   glUseProgram(0);
   ErrCheck("drawLandscape");
}

// Display everything in the scene
void drawSceneContents(const int cam)
{
   // Get the adjusted mvp matrices
   setMVPadjusted(cam);

   // ----- Lighting -----
   // OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   // Enable lighting
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   // Set ambient, diffuse, specular components and position of light 0
   glLightfv(GL_LIGHT0, GL_POSITION, _lightPosition);
   glLightfv(GL_LIGHT0, GL_AMBIENT, _lightAmbient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, _lightDiffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, _lightSpecular);

   // ----- Environment -----
   drawLandscape(cam);
   drawSkyBox();
   
   // ----- Knights -----
   // Draw the general knights
   drawKnight(_knightPositions[0],_knightPositions[1],_knightPositions[2], _mainKnightRot, 0);
   // Draw the knight army
   int u;
   for (u = 1; u < knightCount; u++) {
      // Unit 1
      if ((u-1) < ((knightCount-1)/2)) {
         drawKnight(
            _knightPositions[u*3+0] + _unit1translation[0],
            _knightPositions[u*3+1],
            _knightPositions[u*3+2] + _unit1translation[1],
            0, u);
      }
      // Unit 2
      else {
         drawKnight(
            _knightPositions[u*3+0] + _unit2translation[0],
            _knightPositions[u*3+1],
            _knightPositions[u*3+2] + _unit2translation[1],
            0, u);
      }
   }
   // Draw any knights as particles that we need to (don't need lighting)
   drawKnightPoints(cam);
   // Reset particle count for next iteration
   cleanArray();

   // Show test object
   if (_showTest) testObj((_showTest-1), 15.0);

   glDisable(GL_LIGHT0);
   glDisable(GL_LIGHTING);
   glDisable(GL_NORMALIZE);

   ErrCheck("drawSceneContents");
}

// the contents of my scene will be called in here
void display()
{
   // Clear color buffer & enable depth test
   glEnable(GL_DEPTH_TEST);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   // SPLIT SCREEN?
   if (_showSplitScreen) {
      int seamLoc = (int)(((float)_windowW)*_splitScreenSeam);

      // ----- Camera 1 -----
      glViewport(0, 0, seamLoc, _windowH);

      // Perspective
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      _aspectSaved = _asp*_splitScreenSeam;
      gluPerspective(_fov, _aspectSaved, _worldSize/16.0, 16.0*_worldSize);
      // Modelview
      _cameraLook1[0] = _cameraLoc[0] + Sin(_th[0])*Cos(_ph[0]);
      _cameraLook1[1] = _cameraLoc[1] +             Sin(_ph[0]);
      _cameraLook1[2] = _cameraLoc[2] + Cos(_th[0])*Cos(_ph[0]);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(_cameraLoc[0], _cameraLoc[1], _cameraLoc[2], _cameraLook1[0], _cameraLook1[1], _cameraLook1[2], 0, Cos(_ph[0]), 0);
      // Store the matricies
      glGetFloatv(GL_MODELVIEW_MATRIX, _modelMatrix);
      glGetFloatv(GL_PROJECTION_MATRIX, _projMatrix);
      // Drawing of scene
      drawSceneContents(0);

      // ----- Camera 2 -----
      glViewport(seamLoc + 1, 0, (_windowW - seamLoc), _windowH);

      // Perspective
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      _aspectSaved = _asp*(1.0-_splitScreenSeam);
      gluPerspective(_fov, _aspectSaved, _worldSize/16.0, 16.0*_worldSize);

      // Modelview
      float Ex = _cameraLook2[0] - (_camera2Dist*_worldSize*Sin(_th[1])*Cos(_ph[1]));
      float Ey = _cameraLook2[1] + (_camera2Dist*_worldSize            *Sin(_ph[1]));
      float Ez = _cameraLook2[2] + (_camera2Dist*_worldSize*Cos(_th[1])*Cos(_ph[1]));
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(Ex,Ey,Ez, _cameraLook2[0], _cameraLook2[1], _cameraLook2[2], 0, Cos(_ph[1]), 0);
      // Store the matricies
      glGetFloatv(GL_MODELVIEW_MATRIX, _modelMatrix2);
      glGetFloatv(GL_PROJECTION_MATRIX, _projMatrix2);
      // Drawing of scene
      drawSceneContents(1);
   }
   // MAIN CAMERA ONLY?
   else {
      // Take up whole window
      glViewport(0, 0, _windowW, _windowH);
      // Perspective
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      _aspectSaved = _asp;
      gluPerspective(_fov, _aspectSaved, _worldSize/16.0, 16.0*_worldSize);
      // Modelview
      _cameraLook1[0] = _cameraLoc[0] + Sin(_th[0])*Cos(_ph[0]);
      _cameraLook1[1] = _cameraLoc[1] +             Sin(_ph[0]);
      _cameraLook1[2] = _cameraLoc[2] + Cos(_th[0])*Cos(_ph[0]);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(_cameraLoc[0], _cameraLoc[1], _cameraLoc[2], _cameraLook1[0], _cameraLook1[1], _cameraLook1[2], 0, Cos(_ph[0]), 0);
      // Store the matricies
      glGetFloatv(GL_MODELVIEW_MATRIX, _modelMatrix);
      glGetFloatv(GL_PROJECTION_MATRIX, _projMatrix);
      // Drawing of scene
      drawSceneContents(2);
   }

   // ----- FPS Calculation -----
   // Tick counting
   _thisFrame = SDL_GetTicks();
   _tickCount += (_thisFrame - _lastFrame);
   _lastFrame = _thisFrame;
   // Calculate fps every 15 frames
   const int framesPerCalc = 15;
   if (_frameCount % framesPerCalc == 0) {
      _fps = 1.0/(((float)((float)_tickCount/(float)framesPerCalc))/1000.0);
      _tickCount = 0;
   }
   _frameCount++;
   _lastFrame = _thisFrame;

   // Render scene
   ErrCheck("display");
   glFlush();
   SDL_GL_SwapBuffers();
}

// Mouse in control of a camera
void mouseCamera(const int cam)
{
   // Ignore this frame?
   if (_ignoreJump) {
      SDL_WarpMouse(_windowW/2, _windowH/2);
      _ignoreJump = 0;
      return;
   }

   // Detect mouse position
   int mouseX = 0;
   int mouseY = 0;
   int *mX = &mouseX;
   int *mY = &mouseY;
   SDL_GetMouseState(mX, mY);

   // Get mouse change from center of screen & apply to cam angles
   float thChange = _cameraRotSpeed*(float)((_windowW/2) - mouseX);
   float phChange = _cameraRotSpeed*(float)((_windowH/2) - mouseY);
   _th[cam] += thChange;
   _ph[cam] += phChange;

   // If the cam rotated, check to see if we need to change the particle image
   if ((thChange != 0.0) || (phChange != 0.0)) selectParticleImg(_th[0], _ph[0]);

   // Limit ph & th
   if (_ph[cam] < _minPh) _ph[cam] = _minPh;
   if (_ph[cam] > _maxPh) _ph[cam] = _maxPh;
   _th[cam] = fmod(_th[cam], 360);

   // Move mouse back to center
   SDL_WarpMouse(_windowW/2, _windowH/2);

   // Cannot drag seam in this mode
   _canDragSeam = 0;
}

// Mouse actions with no camera selected (showing cursor)
void mouseFree()
{
   // Detect mouse position
   int mouseX = 0;
   int mouseY = 0;
   int *mX = &mouseX;
   int *mY = &mouseY;
   SDL_GetMouseState(mX, mY);

   // Get mouse position in terms of normalized window position (x axis only)
   float nX = (float)mouseX/(float)_windowW;

   if (!_draggingSeam) {
      // on top of seam?
      if (((nX - _splitScreenSeam) < _splitScreenSeamWidth)
         && ((nX - _splitScreenSeam) > -_splitScreenSeamWidth)
         && _showSplitScreen) {
         SDL_SetCursor(_cursorPreDrag);
         _canDragSeam = 1;
      }
      else {
         SDL_SetCursor(_cursorArrow);
         // Which side are we on?
         if (nX > _splitScreenSeam) _canDragSeam = 5;
         else _canDragSeam = -5;
      }
   }
   else {
      SDL_SetCursor(_cursorDragging);
      _splitScreenSeam = nX;
      if (_splitScreenSeam < _splitScreenSeamLimits[0]) _splitScreenSeam = _splitScreenSeamLimits[0];
      if (_splitScreenSeam > _splitScreenSeamLimits[1]) _splitScreenSeam = _splitScreenSeamLimits[1];
   }
}

// Mouse detection for dragging/clicking
void mouseDown() {
   // Don't do anything in this state
   if (_canDragSeam == 0) return;
   // If split screen, select the side we chose
   if (_showSplitScreen) {
      if (_canDragSeam == 5) {
         _selectedScreen = 1;
         SDL_SetCursor(_cursorNone);
         SDL_WarpMouse(_windowW/2, _windowH/2);
         _ignoreJump = 1;
      }
      else if (_canDragSeam == -5) {
         _selectedScreen = 0;
         SDL_SetCursor(_cursorNone);
         SDL_WarpMouse(_windowW/2, _windowH/2);
         _ignoreJump = 1;
      }
      // Start dragging
      else _draggingSeam = 1;
   }
   else {
      _selectedScreen = 0;
      SDL_SetCursor(_cursorNone);
      SDL_WarpMouse(_windowW/2, _windowH/2);
      _ignoreJump = 1;
   }
}
void mouseUp() {
   // Stop dragging
   _draggingSeam = 0;
}

// Get key events
// This is a combination of what use to be three functions: keyUp, keyDown, & processKeys
void keys()
{
   // Get the key state
   Uint8* keyState = SDL_GetKeyState(NULL);

   // ---------- << PRESS >> ----------
   // ----- Single-press keys -----
   //  Stop program on ESC
   if (keyState[SDLK_ESCAPE]) _running = 0;
   // Set to default on enter
   if (keyState[SDLK_RETURN]) {
      if (!_keyPressed[6])
         setLightDefault();
      else
         setOrientationDefault();
   }
   // Z = free cursor
   if (keyState[SDLK_z]) {
      _selectedScreen = -1;
      SDL_SetCursor(_cursorArrow);
   }
   // ----- Action-per-press keys -----
   // X = change control between main & secondary camera
   if (keyState[SDLK_x] && !_keyPressed[7]) {
      if ((_selectedScreen != -1) && (_showSplitScreen)) _selectedScreen = 1 - _selectedScreen;
      _keyPressed[7] = 1;
   }
   // C = disable/enable split screen
   if (keyState[SDLK_c] && !_keyPressed[8]) {
      _showSplitScreen = 1 - _showSplitScreen;
      _selectedScreen = 0;
      _keyPressed[8] = 1;
      SDL_SetCursor(_cursorNone);
   }
   // T = switch terrian mode
   if (keyState[SDLK_t] && !_keyPressed[10]) {
      _terrianMode = 1 - _terrianMode;
      _keyPressed[10] = 1;
   }
   // G = forced terrian LOD
   if (keyState[SDLK_g] && !_keyPressed[11]) {
      _tForcedLOD = 1 - _tForcedLOD;
      _keyPressed[11] = 1;
   }
   // Y&H = Increase/Decrease terrian LOD
   if (keyState[SDLK_y] && !_keyPressed[12]) {
      if (_tForcedLOD) {
         _tLOD += 1;
         if (_tLOD > _maxTLOD) _tLOD = _maxTLOD;
      }
      _keyPressed[12] = 1;
   }
   if (keyState[SDLK_h] && !_keyPressed[13]) {
      if (_tForcedLOD) {
         _tLOD -= 1;
         if (_tLOD <= 1) _tLOD = 1;
      }
      _keyPressed[13] = 1;
   }
   // U = Show test object
   if (keyState[SDLK_u] && !_keyPressed[18]) {
      _showTest++;
      if (_showTest > 2) _showTest = 0;
      _keyPressed[18] = 1;
   }
   // O = Alter which unit you have control of
   if (keyState[SDLK_o] && !_keyPressed[19]) {
      _controledUnit = 1 - _controledUnit;
      _keyPressed[19] = 1;
   }
   // ----- Hold-press keys -----
   // Q/W/E/A/S/D = Move main camera
   if (keyState[SDLK_w] && !_keyPressed[0]) _keyPressed[0] = 1;
   if (keyState[SDLK_s] && !_keyPressed[1]) _keyPressed[1] = 1;
   if (keyState[SDLK_a] && !_keyPressed[2]) _keyPressed[2] = 1;
   if (keyState[SDLK_d] && !_keyPressed[3]) _keyPressed[3] = 1;
   if (keyState[SDLK_q] && !_keyPressed[4]) _keyPressed[4] = 1;
   if (keyState[SDLK_e] && !_keyPressed[5]) _keyPressed[5] = 1;
   // Shift = Speed up
   if (keyState[SDLK_LSHIFT] && !_keyPressed[6]) _keyPressed[6] = 1;
   // V&B = Rotate main knight
   if (keyState[SDLK_v] && !_keyPressed[14]) _keyPressed[14] = 1;
   if (keyState[SDLK_b] && !_keyPressed[15]) _keyPressed[15] = 1;
   // R&F = Change sun location
   if (keyState[SDLK_r] && !_keyPressed[16]) _keyPressed[16] = 1;
   if (keyState[SDLK_f] && !_keyPressed[17]) _keyPressed[17] = 1;
   // I/K/J/L = Translate unit of knights
   if (keyState[SDLK_i] && !_keyPressed[20]) _keyPressed[20] = 1;
   if (keyState[SDLK_k] && !_keyPressed[21]) _keyPressed[21] = 1;
   if (keyState[SDLK_j] && !_keyPressed[22]) _keyPressed[22] = 1;
   if (keyState[SDLK_l] && !_keyPressed[23]) _keyPressed[23] = 1;
   // ---------- << END OF PRESS >> ----------


   // ---------- << RELEASE >> ----------
   // ----- Action-per-press keys -----
   // Alter splitscreen settings
   if (!keyState[SDLK_x]) _keyPressed[7] = 0;
   if (!keyState[SDLK_c]) _keyPressed[8] = 0;
   // Alter environment settings
   if (!keyState[SDLK_t]) _keyPressed[10] = 0;
   if (!keyState[SDLK_g]) _keyPressed[11] = 0;
   if (!keyState[SDLK_y]) _keyPressed[12] = 0;
   if (!keyState[SDLK_h]) _keyPressed[13] = 0;
   // U = Test object
   if (!keyState[SDLK_u]) _keyPressed[18] = 0;
   // O = Alternate unit control
   if (!keyState[SDLK_o]) _keyPressed[19] = 0;
   // ----- Hold-press keys -----
   // Main camera movement
   if (!keyState[SDLK_w]) _keyPressed[0] = 0;
   if (!keyState[SDLK_s]) _keyPressed[1] = 0;
   if (!keyState[SDLK_a]) _keyPressed[2] = 0;
   if (!keyState[SDLK_d]) _keyPressed[3] = 0;
   if (!keyState[SDLK_q]) _keyPressed[4] = 0;
   if (!keyState[SDLK_e]) _keyPressed[5] = 0;
   // Shift
   if (!keyState[SDLK_LSHIFT] && _keyPressed[6]) _keyPressed[6] = 0;
   // Main knight rotation
   if (!keyState[SDLK_v]) _keyPressed[14] = 0;
   if (!keyState[SDLK_b]) _keyPressed[15] = 0;
   // R&F = Change sun location
   if (!keyState[SDLK_r]) _keyPressed[16] = 0;
   if (!keyState[SDLK_f]) _keyPressed[17] = 0;
   // I/K/J/L = Translate unit of knights
   if (!keyState[SDLK_i]) _keyPressed[20] = 0;
   if (!keyState[SDLK_k]) _keyPressed[21] = 0;
   if (!keyState[SDLK_j]) _keyPressed[22] = 0;
   if (!keyState[SDLK_l]) _keyPressed[23] = 0;
   // ---------- << REND OF ELEASE >> ----------


   // ---------- << PROCESS PRESSED KEYS >> ----------
   // Do something depending on the current state of pressed keys
   // Mostly movement stuff
   // In control of main camera
   if (_selectedScreen == 0) {
      // Shift = speed up
      float traSpeed;
      float eleSpeed;
      if (_keyPressed[6]) {
         traSpeed = _travelSpeed[1];
         eleSpeed = _elevationSpeed[1];
      }
      else {
         traSpeed = _travelSpeed[0];
         eleSpeed = _elevationSpeed[0];
      }
      // Main camera movement
      if (_keyPressed[0]) {
         _cameraLoc[0] += Sin(_th[0])*Cos(_ph[0]) * traSpeed;
         _cameraLoc[1] +=             Sin(_ph[0]) * traSpeed;
         _cameraLoc[2] += Cos(_th[0])*Cos(_ph[0]) * traSpeed;
      }
      if (_keyPressed[1]) {
         _cameraLoc[0] -= Sin(_th[0])*Cos(_ph[0]) * traSpeed;
         _cameraLoc[1] -=             Sin(_ph[0]) * traSpeed;
         _cameraLoc[2] -= Cos(_th[0])*Cos(_ph[0]) * traSpeed;
      }
      if (_keyPressed[2]) {
         _cameraLoc[0] += Sin(_th[0] + 90.0) * traSpeed;
         _cameraLoc[2] += Cos(_th[0] + 90.0) * traSpeed;
      }
      if (_keyPressed[3]) {
         _cameraLoc[0] -= Sin(_th[0] + 90.0) * traSpeed;
         _cameraLoc[2] -= Cos(_th[0] + 90.0) * traSpeed;
      }
      // Q&E = elevation, which is independent of camera angle
      if (_keyPressed[4]) {
         _cameraLoc[1] += eleSpeed;
      }
      if (_keyPressed[5]) {
         _cameraLoc[1] -= eleSpeed;
      }
   }
   // In control of secondary camera
   if (_selectedScreen == 1) {
      if (_keyPressed[1]) {
         _camera2Dist += _camera2DistSpeed;
         if (_camera2Dist > _camera2DistLimits[1])
            _camera2Dist = _camera2DistLimits[1];
      }
      if (_keyPressed[0]) {
         _camera2Dist -= _camera2DistSpeed;
         if (_camera2Dist < _camera2DistLimits[0])
            _camera2Dist = _camera2DistLimits[0];
      }
   }
   // General rotation
   if (_keyPressed[14]) _mainKnightRot += _mainKnightRotSpeed;
   if (_keyPressed[15]) _mainKnightRot -= _mainKnightRotSpeed;
   // Sun movement
   if (_keyPressed[16]) {
      _lightAngle += _lightAngleSpeed;
      _lightPosition[0] = Sin(_lightAngle);
      _lightPosition[2] = Cos(_lightAngle);
   }
   if (_keyPressed[17]) {
      _lightAngle -= _lightAngleSpeed;
      _lightPosition[0] = Sin(_lightAngle);
      _lightPosition[2] = Cos(_lightAngle);
   }
   // Unit translation
   // I = forward
   if (_keyPressed[20]) {
      // Unit 1
      if (!_controledUnit) {
         _unit1translation[0] -= _translationSpeed;
         if (_unit1translation[0] < -_translationMax[0])
            _unit1translation[0] = -_translationMax[0];
      }
      // Unit 2
      else {
         _unit2translation[0] -= _translationSpeed;
         if (_unit2translation[0] < -_translationMax[0])
            _unit2translation[0] = -_translationMax[0];
      }
   }
   // K = backwards
   if (_keyPressed[21]) {
      // Unit 1
      if (!_controledUnit) {
         _unit1translation[0] += _translationSpeed;
         if (_unit1translation[0] > _translationMax[0])
            _unit1translation[0] = _translationMax[0];
      }
      // Unit 2
      else {
         _unit2translation[0] += _translationSpeed;
         if (_unit2translation[0] > _translationMax[0])
            _unit2translation[0] = _translationMax[0];
      }
   }
   // J = left
   if (_keyPressed[22]) {
      // Unit 1
      if (!_controledUnit) {
         _unit1translation[1] += _translationSpeed;
         if (_unit1translation[1] > _translationMax[1])
            _unit1translation[1] = _translationMax[1];
      }
      // Unit 2
      else {
         _unit2translation[1] += _translationSpeed;
         if (_unit2translation[1] > _translationMax[1])
            _unit2translation[1] = _translationMax[1];
      }
   }
   // L = right
   if (_keyPressed[23]) {
      // Unit 1
      if (!_controledUnit) {
         _unit1translation[1] -= _translationSpeed;
         if (_unit1translation[1] < -_translationMax[1])
            _unit1translation[1] = -_translationMax[1];
      }
      // Unit 2
      else {
         _unit2translation[1] -= _translationSpeed;
         if (_unit2translation[1] < -_translationMax[1])
            _unit2translation[1] = -_translationMax[1];
      }
   }
   // ---------- << END OF KEY PROCESSING >> ----------

   ErrCheck("keys");
}

// Window resize
void reshape(int width, int height)
{
   // Ratio of the width to the height of the window
   _asp = (height>0) ? (float)width/height : 1;

   // Set size of the window
   _windowW = (float)(width);
   _windowH = (float)(height);

   ErrCheck("reshape");
}

// Set up the xyz for every knight
// Create a unit of knights 50x6
void setUpKnightPos()
{
   const int unitW = 10;
   const int unitH = 5;
   const float spacing = 2.5;
   const float startX = -((unitH - 1.0)*spacing)/2.0;
   const float startY = +1.2;
   const float startZ = -((unitW - 1.0)*spacing)/2.0;
   unsigned int s;
   unsigned int t;
   // Set 'general' position
   _knightPositions[0] = 0.0;
   _knightPositions[1] = startY;
   _knightPositions[2] = 0.0;
   // Set army positions
   for (s = 1; s < knightCount; s++) {
      t = ((s-1)%((knightCount-1)/2));
      _knightPositions[s*3 + 0] = startX + spacing*(float)(t/unitW);
      _knightPositions[s*3 + 1] = startY;
      _knightPositions[s*3 + 2] = startZ + spacing*(float)(t%unitW);
   }
}


// Load each knight picture
void setUpKnightParticleImages()
{
   glActiveTexture(GL_TEXTURE9);
   unsigned int f;
   unsigned int thC = 0;
   unsigned int phC = 0;
   for (f = 0; f < particleImgCount; f++) {
      // Generate the image name
      char desiredImage[70] = "generated/knight_";
      char phStr[5];
      char thStr[5];
      snprintf(phStr, 5, "%d", phC);
      snprintf(thStr, 5, "%d", thC);
      strcat(desiredImage, phStr);
      strcat(desiredImage, "_");
      strcat(desiredImage, thStr);
      strcat(desiredImage, ".bmp");
      //printf("%s\n", desiredImage); // (For testing)
      // Load the imager
      _knightParticleImgs[f] = LoadTexBMP(desiredImage);
      // Increment th&ph count
      thC += _thPerPic;
      if (thC >= 360) {
         thC = 0;
         phC += _phPerPic;
      }
   }
}

// Create the shader programs
// Also creates our plane that will be tessellated
void createTerrianShaders()
{
   int i;
   float dP; // Distance between vertices

   // Set up the plane that will be tessellated into the environment
   dP = 1.0/(float)gridSize;
   unsigned int gpPlusOne = gridSize + 1;
   // Init verts
   for (i = 0; i < (gpPlusOne)*(gpPlusOne); i++) {
      _tVerts[(i*3) + 0] = -0.5 + (dP * (float)(i%gpPlusOne));
      _tVerts[(i*3) + 1] = 0.0;
      _tVerts[(i*3) + 2] = -0.5 + (dP * (float)(i/gpPlusOne));
      // Uncomment for testing
      //printf("(%d)%f,%f,%f\n",i,_tVerts[(i*3) + 0],_tVerts[(i*3) + 1],_tVerts[(i*3) + 2]);
   }
   // Init faces
   for (i = 0; i < (gridSize)*(gpPlusOne); i++) {
      if ((i + 1) % gpPlusOne == 0) continue; // Dont make faces between ends of rows
      _tFaces[(i*4) + 0] = i;
      _tFaces[(i*4) + 1] = i + gpPlusOne;
      _tFaces[(i*4) + 2] = i + gpPlusOne + 1;
      _tFaces[(i*4) + 3] = i + 1;
   }

   // ----- Program 1: Terrian non-forced -----
   int prog1 = glCreateProgram();
   // Compile shaders
   CreateShader(prog1,GL_VERTEX_SHADER         , "terrain/terrain.vert");
   CreateShader(prog1,GL_TESS_CONTROL_SHADER   , "terrain/dependent.tcs");
   CreateShader(prog1,GL_TESS_EVALUATION_SHADER, "terrain/terrain.tes");
   CreateShader(prog1,GL_FRAGMENT_SHADER       , "terrain/terrain.frag");
   // Associate _Positions with VBO
   glBindAttribLocation(prog1,_Positions,"Position");
   // Link program
   glLinkProgram(prog1);
   // Check for errors
   PrintProgramLog(prog1);
   // Return program #
   _environmentShad0 = prog1;
   // ----- Program 2: Quads non-forced -----
   int prog2 = glCreateProgram();
   // Compile shaders
   CreateShader(prog2,GL_VERTEX_SHADER         , "terrain/terrain.vert");
   CreateShader(prog2,GL_TESS_CONTROL_SHADER   , "terrain/dependent.tcs");
   CreateShader(prog2,GL_TESS_EVALUATION_SHADER, "terrain/quads.tes");
   CreateShader(prog2,GL_GEOMETRY_SHADER       , "terrain/quads.geom");
   CreateShader(prog2,GL_FRAGMENT_SHADER       , "terrain/quads.frag");
   // Associate _Positions with VBO
   glBindAttribLocation(prog2,_Positions,"Position");
   // Link program
   glLinkProgram(prog2);
   // Check for errors
   PrintProgramLog(prog2);
   // Return program #
   _environmentShad1 = prog2;
   // ----- Program 1: Terrian non-forced -----
   int prog3 = glCreateProgram();
   // Compile shaders
   CreateShader(prog3,GL_VERTEX_SHADER         , "terrain/terrain.vert");
   CreateShader(prog3,GL_TESS_CONTROL_SHADER   , "terrain/forced.tcs");
   CreateShader(prog3,GL_TESS_EVALUATION_SHADER, "terrain/terrain.tes");
   CreateShader(prog3,GL_FRAGMENT_SHADER       , "terrain/terrain.frag");
   // Associate _Positions with VBO
   glBindAttribLocation(prog3,_Positions,"Position");
   // Link program
   glLinkProgram(prog3);
   // Check for errors
   PrintProgramLog(prog3);
   // Return program #
   _environmentShad2 = prog3;
   // ----- Create program 4 -----
   int prog4 = glCreateProgram();
   // Compile shaders
   CreateShader(prog4,GL_VERTEX_SHADER         , "terrain/terrain.vert");
   CreateShader(prog4,GL_TESS_CONTROL_SHADER   , "terrain/forced.tcs");
   CreateShader(prog4,GL_TESS_EVALUATION_SHADER, "terrain/quads.tes");
   CreateShader(prog4,GL_GEOMETRY_SHADER       , "terrain/quads.geom");
   CreateShader(prog4,GL_FRAGMENT_SHADER       , "terrain/quads.frag");
   // Associate _Positions with VBO
   glBindAttribLocation(prog4,_Positions,"Position");
   // Link program
   glLinkProgram(prog4);
   // Check for errors
   PrintProgramLog(prog4);
   // Return program #
   _environmentShad3 = prog4;

   // Create the VAO:
   unsigned int vao,verts,faces;
   _Nterr = sizeof(_tFaces)/sizeof(float);
   glGenVertexArrays(1,&vao);
   glBindVertexArray(vao);

   // Create the VBO for positions:
   glGenBuffers(1,&verts);
   glBindBuffer(GL_ARRAY_BUFFER,verts);
   glBufferData(GL_ARRAY_BUFFER,sizeof(_tVerts),_tVerts,GL_STATIC_DRAW);
   glEnableVertexAttribArray(_Positions);
   glVertexAttribPointer(_Positions,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);

   // Create the VBO for facet indices:
   glGenBuffers(1,&faces);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,faces);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(_tFaces),_tFaces,GL_STATIC_DRAW);
}

void createKnightParticleShader()
{
   // Create program
   int prog = glCreateProgram();
   // Compile and add shaders
   CreateShader(prog,GL_VERTEX_SHADER, "knight/knightParticle.vert");
   CreateShader(prog,GL_GEOMETRY_SHADER, "knight/knightParticle.geom");
   CreateShader(prog,GL_FRAGMENT_SHADER, "knight/knightParticle.frag");
   // Link program
   glLinkProgram(prog);
   // Check for errors
   PrintProgramLog(prog);
   _knightParticleShader = prog;
}

// Called if something goes wrong
void fatal(char *str)
{
   printf("%s\n", str);
   _running = 0;
}

// Main function
int main(int argc, char* argv[])
{
   // ----- Window init -----
   // SDL event
   SDL_Event event;

   // Set position of window
   if (SDL_putenv("SDL_VIDEO_WINDOW_POS=100,30")) fatal("Setting window position failed");

   // Initialize window
   SDL_Init(SDL_INIT_VIDEO);
   const int startW = 1100;
   const int startH = 650;
   _window = SDL_SetVideoMode(startW, startH, 0, SDL_OPENGL|SDL_RESIZABLE|SDL_DOUBLEBUF);
   if (!_window) fatal("SDL_Surface init failed.\n");
   reshape(startW, startH);

   #ifdef _WIN32
   GLenum err = glewInit();
   if (GLEW_OK != err) fatal("GLEW init failed.\n");
   #endif

   // Set up cursors
   _cursorArrow = init_system_cursor(_arrowCursor, 0, 0);
   _cursorNone = init_system_cursor(_noneCursor, 0, 0);
   _cursorPreDrag = init_system_cursor(_preDragCursor, 16, 16);
   _cursorDragging = init_system_cursor(_draggingCursor, 16, 16);
   // Set cursor to nothing initially
   SDL_SetCursor(_cursorNone);

   // ----- Read settings from settings.txt -----
   FILE *fSETTINGS;
   char fileInfo[15];
   char thSet[6];
   char phSet[6];
   fSETTINGS = fopen("settings.txt", "r");
   fread(fileInfo, sizeof(char), 15, fSETTINGS);
   fclose(fSETTINGS);
   int j = 0;
   while (fileInfo[j] != '_') {
      thSet[j] = fileInfo[j];
      j++;
   }
   thSet[j] = '\0';
   int k = 0;
   while (fileInfo[k+j+1] != '_' && fileInfo[k+j+1] != '\0') {
      phSet[k] = fileInfo[k+j+1];
      k++;
   }
   phSet[k] = '\0';
   _thPerPic = atoi(thSet);
   _phPerPic = atoi(phSet);

   // ----- Texture & Object loading -----
   loadKnightObjects();

   // 0: Skybox texture
   glActiveTexture(GL_TEXTURE0);
   _textures[0] = LoadTexBMP("skybox/skyBoxClouds.bmp");
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This prevents skybox seam
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // This prevents skybox seam
   // 1: Height map
   glActiveTexture(GL_TEXTURE1);
   _textures[1] = LoadTexBMP("terrain/environmentMap.bmp");
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Stops edges of terrian from "falling" off
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Stops edges of terrian from "falling" off
   // 2: Grass
   glActiveTexture(GL_TEXTURE2);
   _textures[2] = LoadTexBMP("terrain/grass.bmp");
   // 3: Rock
   glActiveTexture(GL_TEXTURE3);
   _textures[3] = LoadTexBMP("terrain/rock.bmp");
   // 4: Road
   glActiveTexture(GL_TEXTURE4);
   _textures[4] = LoadTexBMP("terrain/road.bmp");
   // 5: Chain texture
   glActiveTexture(GL_TEXTURE5);
   _textures[5] = LoadTexBMP("knight/chainT.bmp");
   // 6: Chain normal-map texture
   glActiveTexture(GL_TEXTURE6);
   _textures[6] = LoadTexBMP("knight/chainB.bmp");
   // 7: Test texture
   glActiveTexture(GL_TEXTURE7);
   _textures[7] = LoadTexBMP("knight/yo0.bmp");
   // 8: Test normal-map texture
   glActiveTexture(GL_TEXTURE8);
   _textures[8] = LoadTexBMP("knight/yo1.bmp");
   // 9: Knight particle image
   setUpKnightParticleImages();
   // 10: Knight details image
   setUpKnightImages();

   // --- Shader compilation ---
   compileKnightPrograms();
   _skyBoxShader = CreateShaderProg("skybox/sky.vert", "skybox/sky.frag");
   createKnightParticleShader();
   createTerrianShaders();

   // Set the title for the first time
   setCaption();
   // Set background color
   glClearColor(0.1, 0.1, 0.1, 1.0);
   // Check for initialization errors
   ErrCheck("init");
   // Set default values
   setOrientationDefault();
   // Set knight positions
   setUpKnightPos();
   cleanArray();

   // Init particle image
   _thUse = -1;
   _phUse = -1;
   selectParticleImg(_th[0], _ph[0]);

   // Init variables
   _phPerTh = 360/_thPerPic;
   _knightScaledHeight = (_landSize/_mountainHeights)/_knightScale;

   // While the program is running
   while (_running)
   {
      SDL_PollEvent(&event);
      switch (event.type)
      {
         // Reshape
         case SDL_VIDEORESIZE:
            _window = SDL_SetVideoMode(event.resize.w, event.resize.h, 0,
               SDL_OPENGL|SDL_RESIZABLE|SDL_DOUBLEBUF);
            reshape(event.resize.w, event.resize.h);
            break;
         // Mouse click
         case SDL_MOUSEBUTTONDOWN:
            mouseDown();
            break;
         // Mouse release
         case SDL_MOUSEBUTTONUP:
            mouseUp();
            break;
         // Quit
         case SDL_QUIT:
            _running = 0;
            break;
         // Nothing
         default:
            break;
      }

      // Process mouse
      if (_selectedScreen == -1) mouseFree(); // Free cursor controls
      else if (_frameCount > 10) mouseCamera(_selectedScreen); // In control of a camera
      else SDL_WarpMouse(_windowW/2, _windowH/2); // To prevent start-up jump
      setCaption(); // Set the caption
      display(); // Draw the scene
      // To guarentee keyboard interaction, key functions are put into main loop rather than poll event
      keys();
   }

   //  Shut down
   SDL_Quit();
   return 0;
}
