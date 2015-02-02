/*
   Zack Diller
   CSCI 5229 FINAL PROJECT
   Files:
   knightArmy.c
   spriteMaker.c
   knightDrawer.c
   > knightDrawer.h

   File Description: Header file for knightDrawer.

   Please see README.txt for more info
*/

//-----------------------------------------------------------------------------

// Function Prototypes:
void drawKnightModel(const float tX, const float tY, const float tZ, const float rot, const float knightH,
    const float landSize, const float knightScale);
void testObj(const int textureUse, const float knightH);
void compileKnightPrograms();
void loadKnightObjects();
void setUpKnightImages();