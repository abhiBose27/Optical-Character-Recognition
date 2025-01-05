#Start

CC=gcc
CFLAGS= `pkg-config --cflags sdl2 --cflags gtk+-3.0` -MMD -Wall -O3
LDLIBS= `pkg-config --libs sdl2 --libs gtk+-3.0` -lSDL2_image -lm -lSDL2
all: UI_code

UI_code: Resize.o CharDetection.o BlackAndWhite.o pixeloperations.o GaussianBlur.o Network.o Layer.o Neuron.o sdl_libs.o deskew.o

Resize.o: Resize.h pixeloperations.h

CharDetection.o: pixeloperations.h CharDetection.h

deskew.o: pixeloperations.h deskew.h

BlackAndWhite.o: pixeloperations.h BlackAndWhite.h

GaussianBlur.o:  GaussianBlur.h pixeloperations.h

pixeloperations.o: pixeloperations.h

Network.o: Network.h Layer.h Neuron.h pixeloperations.h sdl_libs.h

Layer.o: Layer.h Neuron.h

Neuron.o: Neuron.h

sdl_libs.o: sdl_libs.h

clean:
	${RM} *.o
	${RM} *.d
	${RM} UI_code
	${RM} Result

#END
