CC=clang++
CFLAGS =-Wall -std=c++0x -g
OPTFLAGS=
INCLUDES=-I$(PWD)

SOURCES=tinyxml2.cpp WindScenario.cpp WindFarmLayoutEvaluator.cpp KusiakLayoutEvaluator.cpp GA.cpp Chromosome.cpp
OBJECTS=$(SOURCES:.cpp=.o)

eval.o: $(OBJECTS)
	$(CC) $(CFLAGS) -o eval.o $(OBJECTS) main.cpp $(INCLUDES) $(OPTFLAGS)

evaluate: $(OBJECTS)
	$(CC) $(CFLAGS) -o evaluate.o $(OBJECTS) evaluate.cpp $(INCLUDES) $(OPTFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@ -w $(INCLUDES) $(OPTFLAGS)

run: eval.o
	./eval.o Scenarios/practice_0.xml

clean:
	rm -f *.o
