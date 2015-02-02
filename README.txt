Zack Diller
100608238
CSCI 5229
Computer Graphics
Final Assignment
knightArmy.c
spriteMaker.c

MAIN CONTROLS:
W,A,S,D = Movement
Q&E = Alter camera elevation
Shift = Speed up
Z = Free cursor
X = Switch cameras, if splitscreen
C = Disable splitscreen
T = Switch terrian mode
G = Forced terrian LOD
Y/H = Increase/decrease LOD when in forced mode
R/F = Change sun location
V/B = Change general knight rotation (to show off normal-mapping shader, but can also just change sun location)
U = Show normal-mapping test object
O = Switch which unit you have control of
I/J/K/L = Move unit of knights

A "unit" is a group of knights that move in a group, there are two in the scene that can be moved around. There
is also one "general" knight that stands near the origin of the world, he doesn't move and is used in the
splitscreen to show the Level of Detail happening.

IMPORTANT:
- Run spriteMaker before knightArmy!
- Do not move files around

Files:
knightArmy.c:
This creates the main program that displays the scene.
spriteMaker.c:
This program creates the knight particles for the knightArmy program. NOTE: Must run before running knightArmy!
knightDrawer.c:
Draws a full knight model. Since both knightArmy and spriteMaker draws a full knight this file was created.

About:
My final project does a variety of things, its main focus is Level of Detail.

Everything about the program:

- Skybox uses a shader program to move it around, will get behind clouds and light them up
- Environment uses a tessellation shader to be created. As far as the CPU knows, the environment
is just a 64 by 64 plane that extends from -0.5 to -0.5
- The environment tessellation shader has four modes (a combination of the following):
    Two show the environment in a natural-looking setting.
    Two show the environmemt as triangles with outlines to display how the tessellation looks
    Two tessellate depending on the distance from the camera, main component of level of detail
    Two tessellate depending on user-controlled settings
- The main camera-position dependent shaders won't bother tessellating stuff off camera
- The program allows split-screen to happen to allow for a behind-the-scenes of the tessellation
this is happening. The left camera is the "main" camera.
- You can move this split screen seam back and forth aftering getting mouse control back
- The cursors used in this program were custom made by me
- The model is a series of free models found online, I spent many hours splicing them together and
improving them, namely their normals
- The model uses a shader program
- This shader program adjust the knight's height to match that of the environment
- If the main camera moves far enough away from the knight models they become billboarding particles,
the sprites used were the ones made by the sprite maker program
- The CPU organizes these particles in order of how far they from the main camera

Time spent of this program:
>70 hours
...A lot of debugging!