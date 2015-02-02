/*
   Zack Diller
   CSCI 5229 FINAL PROJECT
   Files:
   knightArmy.c
   spriteMaker.c
   > knightDrawer.c
   knightDrawer.h

   File Description: Draws a full knight model. Since both knightArmy and spriteMaker draws a full knight this file was created.

   Please see README.txt for more info
*/
#include "CSCIx239.h"

#ifdef _WIN32
#include <GL/glew.h>
#else
#include <GL/glut.h>
#endif

static unsigned int _KNIGHTmodel[5];
static GLuint _KNIGHTShadChain;
static GLuint _KNIGHTShadShiny;
static GLuint _KNIGHTtexture;

static void Cube()
{
   // Front
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,+1);
   glColor3f(1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,+1);
   glColor3f(0,0,0);
   glTexCoord2f(1,1); glVertex3f(+1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,+1);
   glEnd();
   // Back
   glColor3f(0,0,1);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   // Right
   glColor3f(1,1,0);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   // Left
   glColor3f(1,1,1);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   // Top
   glBegin(GL_QUADS);
   glNormal3f(0,+1, 0);
   glColor3f(0,1,1);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glColor3f(1,1,0);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glColor3f(1,0,1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glColor3f(1,1,1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   // Bottom
   glColor3f(1,0,1);
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
}

// Draw a knight model
void drawKnightModel(const float tX, const float tY, const float tZ, const float rot, const float knightH, const float landSize,
   const float knightScale) {

   int id;

   // Get the texture coordinates of this 
   float xTexCoord = (tX / landSize) + 0.5;
   float zTexCoord = 1.0 - ((tZ / landSize) + 0.5);

   // Apply the desired transformation and get the resulting mvp matrix
   glPushMatrix();
   glTranslatef(tX,tY,tZ);
   glScalef(knightScale,knightScale,knightScale);
   glRotatef(rot, 0, 1, 0);

   // Use knight shader for the respective parts
   // ----- SHINY ARMOR -----
   // Terrian texture
   glUseProgram(_KNIGHTShadShiny);
   // Textures
   id = glGetUniformLocation(_KNIGHTShadShiny,"terrain");
   if (id>=0) glUniform1i(id,1);
   // Knight position in terms of height map
   id = glGetUniformLocation(_KNIGHTShadShiny,"texLoc");
   if (id>=0) glUniform2f(id,xTexCoord,zTexCoord);
   // Adjust for height
   id = glGetUniformLocation(_KNIGHTShadShiny,"heightAdjust");
   if (id>=0) glUniform1f(id,knightH);
   id = glGetUniformLocation(_KNIGHTShadShiny,"texture");
   if (id>=0) glUniform1i(id,10);
   glDisable(GL_CULL_FACE);
   glCallList(_KNIGHTmodel[0]);
   glCallList(_KNIGHTmodel[2]);
   glCallList(_KNIGHTmodel[3]);
   glCallList(_KNIGHTmodel[4]);
   glEnable(GL_CULL_FACE);

   // ----- CHAIN MAIL -----
   glUseProgram(_KNIGHTShadChain);
   // Texures
   id = glGetUniformLocation(_KNIGHTShadChain,"terrain");
   if (id>=0) glUniform1i(id,1);
   id = glGetUniformLocation(_KNIGHTShadChain,"chainText");
   if (id>=0) glUniform1i(id,5);
   id = glGetUniformLocation(_KNIGHTShadChain,"chainBump");
   if (id>=0) glUniform1i(id,6);
   // Knight position in terms of height map
   id = glGetUniformLocation(_KNIGHTShadChain,"texLoc");
   if (id>=0) glUniform2f(id,xTexCoord,zTexCoord);
   // Adjust for height
   id = glGetUniformLocation(_KNIGHTShadChain,"heightAdjust");
   if (id>=0) glUniform1f(id, knightH);
   glCallList(_KNIGHTmodel[1]);

   glUseProgram(0);

   // Done
   glPopMatrix();
   ErrCheck("drawKnightModel");
}
// Draw test object
void testObj(const int textureUse, const float knightH) {
   int id;

   // Constants for where the cubes are drawn
   const float xT = 5.0;
   const float zT = 5.0;

   glUseProgram(_KNIGHTShadChain);

   // Knight position in terms of height map
   const float xTexCoord = 0.0;
   const float zTexCoord = 0.0;
   // Texures
   id = glGetUniformLocation(_KNIGHTShadChain,"terrain");
   if (id>=0) glUniform1i(id,0);
   id = glGetUniformLocation(_KNIGHTShadChain,"chainText");
   if (id>=0) glUniform1i(id,5+textureUse*2);
   id = glGetUniformLocation(_KNIGHTShadChain,"chainBump");
   if (id>=0) glUniform1i(id,6+textureUse*2);
   // Knight position in terms of height map
   id = glGetUniformLocation(_KNIGHTShadChain, "texLoc");
   if (id>=0) glUniform2f(id,xTexCoord, zTexCoord);
   // Adjust for height
   id = glGetUniformLocation(_KNIGHTShadChain, "heightAdjust");
   if (id>=0) glUniform1f(id, knightH);

   // Draw two cubes near the axis origin
   glPushMatrix();
   glTranslatef(xT,15.0,zT);
   Cube();
   glTranslatef(10.0,0.0,0.0);
   Cube();
   glPopMatrix();

   // Done
   glUseProgram(0);
   ErrCheck("testObj");
}
// Compile the knight programs
void compileKnightPrograms() {
   _KNIGHTShadShiny = CreateShaderProg("knight/shiny.vert", "knight/shiny.frag");
   _KNIGHTShadChain = CreateShaderProg("knight/chain.vert", "knight/chain.frag");
}
// Load each knight object
void loadKnightObjects() {
   _KNIGHTmodel[0] = LoadOBJ("knight/knightArmorShiny.obj");
   _KNIGHTmodel[1] = LoadOBJ("knight/knightArmorChain.obj");
   _KNIGHTmodel[2] = LoadOBJ("knight/knightArmorLeather.obj");
   _KNIGHTmodel[3] = LoadOBJ("knight/knightArmorShield.obj");
   _KNIGHTmodel[4] = LoadOBJ("knight/knightArmorSword.obj");
}
// Load knight images
void setUpKnightImages() {
   glActiveTexture(GL_TEXTURE10);
   _KNIGHTtexture = LoadTexBMP("knight/knightDetails.bmp");
}