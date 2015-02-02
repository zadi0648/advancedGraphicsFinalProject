/*
   Zack Diller
   CSCI 5229 FINAL PROJECT
   Files:
   knightArmy.c
   > spriteMaker.c
   knightDrawer.c
   knightDrawer.h

   File Description: This program creates the knight particles for the knightArmy program. NOTE: Must run before running knightArmy!

   Please see README.txt for more info
*/

#include <stdlib.h>
#include "CSCIx239.h"
#include "knightDrawer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#ifdef _WIN32
#include <GL/glew.h>
#else
#include <GL/glut.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL/SDL.h"

// Used to convert the bmp data into a human readable form
union
{
   unsigned int integer;
   unsigned char byte[4];
} conversion;

// Application variables
char _title[200]; // Title of our application
// Projection variables
int _fov = 20; // Field of view
float _worldSize = 1.0; // Size of world
float _asp; // Aspect ratio
// Number of iterations for each axis
int _maxThIters;
int _maxPhIters;
char _currDir[300];
// User-set variables
int _object[10];
unsigned int _numObjects;
char _fileName[100];
int _thIter; // How much th increases per iteration
int _phIter; // How much ph increases per iteration
int _width; // Screen width
int _height; // Screen height
float _scale; // Object size
int _displays = 0;
// Lighting
static const float _lightPosition[] = {0.17365,0.7,0.98481,0.0};
static const float _lightEmission[] = {0.3,0.3,0.3,1.0};
static const float _lightAmbient[] = {0.1,0.1,0.1,1.0};
static const float _lightDiffuse[] = {1.0,1.0,1.0,1.0};
static const float _lightSpecular[] = {1.0,1.0,1.0,1.0};
static const float _lightShinyness[] = {16};

// Set the caption
void setCaption(int th, int ph)
{
   sprintf(_title, "(%d,%d) Parameters(%s,%d,%d,%d,%d,%.2f)", th, ph, _fileName, _thIter, _phIter, _width, _height, _scale);
   SDL_WM_SetCaption(_title, 0);
}

// Draw the current frame and write to the image
void displayAndPrint(int th, int ph)
{
   // Increment counter
   _displays++;
   //printf("%d\n", _displays); // For testing
   // Set up projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(_fov, _asp, _worldSize/16.0, 16.0*_worldSize);

   // Set up modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   double Ex = -2 * Sin(th) * Cos(ph);
   double Ey = +2 * Sin(ph);
   double Ez = -2 * Cos(th) * Cos(ph);
   gluLookAt(Ex,Ey,Ez, 0,0,0, 0,Cos(ph),0);
   
   // Set up lighting
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

   // Clean slate
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   // ----- DRAWING OF OBJECT -----
   glEnable(GL_DEPTH_TEST);
   glPushMatrix();
   glScaled(_scale,_scale,_scale);
   drawKnightModel(0,0,0,0,0.0,0.0,_scale);
   glPopMatrix();
   glDisable(GL_DEPTH_TEST);

   // Disables
   glDisable(GL_LIGHT0);
   glDisable(GL_LIGHTING);
   glDisable(GL_NORMALIZE);

   // Set the caption
   setCaption(th, ph);

   // Render the scene
   glFlush();
   SDL_GL_SwapBuffers();

   // ----- WRITE TO IMAGE -----
   int i;
   // Create the pixel buffer of our scene
   GLubyte *buffer = malloc(3 * _width * _height * sizeof(GLubyte));
   if (buffer) glReadPixels(0,0,_width,_height,GL_BGR,GL_UNSIGNED_BYTE,buffer);

   // Read the "bmpPreClone.bmp" in the project's main directory
   FILE *fREAD;
   FILE *fWRITE;
   char readFile[300] = "";
   char writeFile[300] = "";

   // Get the name of the read file
   strcat(readFile, _currDir);
   strcat(readFile, "/bmpPreClone.bmp");
   // Create the name of the file we will write to
   strcat(writeFile, _currDir);
   strcat(writeFile, "/generated/");
   char smallFileName[50] = "";
   char curVal[10];
   i = 0;
   while (_fileName[i] != '.' && _fileName[i] != '\0') {
      smallFileName[i] = _fileName[i];
      i++;
   }
   strcat(writeFile, smallFileName);
   strcat(writeFile, "_");
   sprintf(curVal, "%d", ph);
   strcat(writeFile, curVal);
   strcat(writeFile, "_");
   sprintf(curVal, "%d", th);
   strcat(writeFile, curVal);
   strcat(writeFile, ".bmp");

   // We got the file names we want, now open them
   fREAD = fopen(readFile, "rb");
   fWRITE = fopen(writeFile, "wb");

   // After we are done copying the file, edit it to show our scene
   unsigned char info[100];
   // Get the current file size - make the size match our scene
   fread(info, sizeof(unsigned char), 54, fREAD);
   // Image size is at 18 & 24 bytes in
   conversion.integer = _width;
   info[18] = conversion.byte[0]; info[19] = conversion.byte[1];
   info[20] = conversion.byte[2]; info[21] = conversion.byte[3];
   conversion.integer = _height;
   info[22] = conversion.byte[0]; info[23] = conversion.byte[1];
   info[24] = conversion.byte[2]; info[25] = conversion.byte[3];
   fwrite(info, sizeof(unsigned char), 54, fWRITE);
   // Info is copied over, now we need to fill in the pixel information
   fread(info, sizeof(unsigned char), 3, fREAD);
   fwrite(buffer, sizeof(GLubyte), 3 * _width * _height, fWRITE);

   // Free the file & buffer
   free(buffer);
   fclose(fREAD);
   fclose(fWRITE);
   // ----- END OF IMAGE WRITING -----

   // Error check
   ErrCheck("displayAndPrint");
}

// Main function
int main(int argc, char* argv[])
{
   // Check given # arguements
   if ((argc != 7) && (argc != 1)) {
      Fatal("Invalid # of arguments given.");
   }
   // Read in all arguements
   else if (argc == 7) {
      strncpy(_fileName, argv[1], strlen(argv[1]));
      _thIter = atoi(argv[2]);
      _phIter = atoi(argv[3]);
      _width = atoi(argv[4]);
      _height = atoi(argv[5]);
      _scale = strtod(argv[6], NULL);
   }
   // Use my 'default settings'
   else {
      strncpy(_fileName, "knight.obj", 10);
      _thIter = 5;
      _phIter = 5;
      _width =  128;
      _height = 128;
      _scale =  0.42;
   }

   // No negative or zero values
   if ((_thIter <= 0) || (_phIter <= 0) || (_width <= 0) || (_height <= 0) || (_scale <= 0.0))
      Fatal("Values must be postive and non-zero.");
   // th & ph iterations okay?
   if (360 % _thIter != 0) Fatal("Arguement 1 must be divisible by 180");

   // Arguements pass - set # of iterations
   _maxThIters = 360 / _thIter;
   _maxPhIters = 90 / _phIter;

   // Set aspect ratio
   _asp = ((float)_width/(float)_height);

   // Initialize window
   SDL_Surface* window; // The window pointer
   SDL_Init(SDL_INIT_VIDEO); //  Initialize SDL
   window = SDL_SetVideoMode(_width, _height, 0, SDL_OPENGL|SDL_DOUBLEBUF);
   if (!window) Fatal("SDL init failed.");
   #ifdef _WIN32
   GLenum err = glewInit();
   if (GLEW_OK != err) Fatal("GLEW init failed.\n");
   #endif

   // Chroma key
   glClearColor(0, 1, 0, 1.0);

   // Set the caption for the first time
   setCaption(0,0);

   // Load the objects and the image, and create the programs
   loadKnightObjects();
   setUpKnightImages();
   compileKnightPrograms();

   // See if the 'generated' directory exists
   getcwd(_currDir, 300); // Store the current directory
   char command[300] = "";
   strcat(command, _currDir);
   strcat(command, "/generated");
   struct stat st = {0};
   // Doesn't exist - create it. Note that ubuntu and windows use different versions.
   if (stat(command, &st) == -1) {
#ifdef _WIN32
      mkdir(command);
#else
      mkdir(command, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
#endif
   }
   // Does exist - clean it out
   else {
      char rmCommand[300] = "rm -r ";
      strcat(rmCommand, command);
      strcat(rmCommand, "/*");

      //printf("%s\n\n\n", rmCommand); // For testing
      system(rmCommand);
   }

   // Write the settings we used into the settings file for knightArmy.exe
   char settingsFile[300] = "";
   strcat(settingsFile, _currDir);
   strcat(settingsFile, "/settings.txt");
   FILE *fSETTINGS;
   fSETTINGS = fopen(settingsFile, "w");
   char settings[10];
   sprintf(settings, "%d_%d_", _thIter, _phIter);
   int u = 0;
   while (settings[u] != '\0') u++;
   fwrite(settings, sizeof(char), u, fSETTINGS);
   fclose(fSETTINGS);

   // While the program is running
   // Note that this program has no user interaction during runtime
   int ph,th;
   for (ph = 90; ph >= 0; ph -= _phIter)
      for (th = 0; th < 360; th += _thIter)
         displayAndPrint(th, ph);

   // Shut down
   SDL_Quit();
   return 0;
}
