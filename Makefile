CC = g++
OBJECTS = Actors.o CaveBuilder.o CaveSites.o CaveSystem.o
SRCDIR = ./src
ODIR = ./Output
UTILS_PATH = $(SRCDIR)/utils
OBJ_PATHS = $(patsubst %,$(ODIR)/%,$(OBJECTS))

build: $(OBJECTS)
	$(CC) -o $(ODIR)/main ./main.cpp $(OBJ_PATHS)

Actors.o: CaveSites.o
	$(CC) -o $(ODIR)/$@ -c $(SRCDIR)/Actors/Actors.cpp

CaveBuilder.o: CaveSites.o
	$(CC) -o $(ODIR)/$@ -c $(SRCDIR)/CaveBuilder/CaveBuilder.cpp

CaveSites.o:
	$(CC) -o $(ODIR)/$@ -c $(SRCDIR)/CaveSites/CaveSites.cpp

CaveSystem.o: Actors.o CaveBuilder.o
	$(CC) -o $(ODIR)/$@ -c $(SRCDIR)/CaveSystem/CaveSystem.cpp

clean:
	rm $(ODIR)/main
	rm $(OBJ_PATHS)
	rm $(ODIR)/*.txt

run:
	./$(ODIR)/main
