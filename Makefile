#  Make file for Zack Diller Advance Grahpics Final Project

#  Executables
EX=knightArmy spriteMaker

#  Libraries - Linux
#LIBS=-lglut -lGLU -lSDLmain -lSDL
#  Libraries - MinGW
LIBS=-lglew32 -lglu32 -lopengl32 -lmingw32 -lSDLmain -lSDL -lSDL_mixer -lz

#  Main target
all: $(EX)

#  Generic compile rules
.c.o:
	gcc -c -O3 -Wall $<
.cpp.o:
	g++ -c -O3 -Wall $<

#  Generic compile and link
%: %.c CSCIx239.a knightDrawer.a
	gcc -Wno-unused-result -fno-strict-aliasing -Wall -O3 -o $@ $^ $(LIBS)

#  Clean; including generated directory
clean:
	rm -rf $(EX) *.o *.a *.exe stderr.txt stdout.txt settings.txt generated

#  Create archives
CSCIx239.a: fatal.o loadtexbmp.o project.o errcheck.o object.o elapsed.o shader.o
	ar -rcs CSCIx239.a $^
knightDrawer.a: knightDrawer.o loadtexbmp.o project.o errcheck.o object.o elapsed.o shader.o
	ar -rcs knightDrawer.a $^