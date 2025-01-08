#Start

CC=gcc
CFLAGS= `pkg-config --cflags sdl2 --cflags gtk+-3.0` -MMD -Wall -O3
LDLIBS= `pkg-config --libs sdl2 --libs gtk+-3.0` -lSDL2_image -lm -lSDL2
all: UI_code

UI_code: CharDetection.o pixeloperations.o Network.o Layer.o Neuron.o sdl_libs.o Kernels.o PreProcessing.o

CharDetection.o: pixeloperations.h CharDetection.h

pixeloperations.o: pixeloperations.h

Network.o: Network.h Layer.h Neuron.h pixeloperations.h sdl_libs.h

Layer.o: Layer.h Neuron.h

Neuron.o: Neuron.h

Kernels.o: Kernels.h

PreProcessing.o: PreProcessing.h Kernels.h

sdl_libs.o: sdl_libs.h

clean:
	${RM} *.o
	${RM} *.d
	${RM} UI_code
	${RM} Result

#END
