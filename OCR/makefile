#Start

CC=gcc
CFLAGS= `pkg-config --cflags sdl2 --cflags gtk+-3.0` -MMD -Wall -O3
LDLIBS= `pkg-config --libs sdl2 --libs gtk+-3.0` -lSDL2_image -lm -lSDL2
all: UI_code

UI_code: pixeloperations.o Network.o Layer.o Neuron.o sdl_libs.o Kernels.o PreProcessing.o TextReconstruction.o TextDestruction.o

TrainModel: pixeloperations.o Network.o Layer.o Neuron.o sdl_libs.o Dataset.o

TestPreProcessing: pixeloperations.o sdl_libs.o PreProcessing.o

TextDestruction.o: TextDestruction.h

TextReconstruction.o: TextReconstruction.h

pixeloperations.o: pixeloperations.h

Network.o: Network.h

Dataset.o: Dataset.h

Layer.o: Layer.h

Neuron.o: Neuron.h

Kernels.o: Kernels.h

PreProcessing.o: PreProcessing.h

sdl_libs.o: sdl_libs.h

clean:
	${RM} *.o
	${RM} *.d
	${RM} UI_code
	${RM} Result
	${RM} TrainModel
	${RM} TestPreProcessing

#END
