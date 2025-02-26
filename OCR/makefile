#Start

CC=gcc
CFLAGS= `pkg-config --cflags sdl2 --cflags gtk+-3.0` -MMD -Wall -O3
LDLIBS= `pkg-config --libs sdl2 --libs gtk+-3.0` -lSDL2_image -lm -lSDL2
all: UI_code

UI_code: PixelOperations.o Network.o Layer.o Neuron.o SdlLibs.o Kernels.o PreProcessing.o TextReconstruction.o TextDestruction.o

TrainModel: PixelOperations.o Network.o Layer.o Neuron.o SdlLibs.o Dataset.o

TestPreProcessing: PixelOperations.o SdlLibs.o PreProcessing.o

TextDestruction.o: TextDestruction.h

TextReconstruction.o: TextReconstruction.h

PixelOperations.o: PixelOperations.h

Network.o: Network.h

Dataset.o: Dataset.h

Layer.o: Layer.h

Neuron.o: Neuron.h

Kernels.o: Kernels.h

PreProcessing.o: PreProcessing.h

SdlLibs.o: SdlLibs.h

clean:
	${RM} *.o
	${RM} *.d
	${RM} UI_code
	${RM} Result
	${RM} TrainModel
	${RM} TestPreProcessing

#END
