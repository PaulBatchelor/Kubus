INCLUDES=

system ?= $(shell uname -s)

ifeq ($(system), Darwin)
CXX=clang++
FLAGS=-D__MACOSX_CORE__ -c
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon  -framework OpenGL \
	-framework GLUT -framework Foundation \
	-framework AppKit -lstdc++ -lm
else
CXX=g++
FLAGS=-D__UNIX_JACK__ -c
LIBS= -ljack -lGL -lGLU -lglut -lstdc++ -lm -lpthread
endif

OBJS= RtAudio.o kubus.o hsl.o draw.o windows.o rms.o port.o inih/ini.o butbp.o

RMS_FLAGS=-DSPFLOAT=float -DSP_OK=1 -DSP_NOT_OK=0 
CFLAGS=$(RMS_FLAGS) -c -lm
FLAGS+= $(RMS_FLAGS)

# KissFFT flags

ifdef SR
FLAGS += -DMY_SRATE=$(SR)
endif

# FLAGS += -O3 -ffast-math -Wall -g -Dkiss_fft_scalar=float 
CFLAGS += -O3 -Wall -g -Dkiss_fft_scalar=float 
OBJS += kissfft/kiss_fft.o kissfft/kiss_fftr.o

default: kubus

kubus: $(OBJS)
	$(CXX) -o kubus $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(FLAGS) $^

clean:
	rm -f $(OBJS) kubus
