CXX=g++
INCLUDES=
# FLAGS=-D__MACOSX_CORE__ -c
FLAGS=-D__UNIX_JACK__ -c
#LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \ -framework IOKit -framework Carbon  -framework OpenGL \
#	-framework GLUT -framework Foundation \
#	-framework AppKit -lstdc++ -lm

LIBS= -ljack -lGL -lGLU -lglut -lstdc++ -lm -lpthread

OBJS= RtAudio.o kubus.o draw.o windows.o

# KissFFT flags

ifdef SR
FLAGS += -DMY_SRATE=$(SR)
endif

# FLAGS += -O3 -ffast-math -Wall -g -Dkiss_fft_scalar=float 
FLAGS += -O3 -Wall -g -Dkiss_fft_scalar=float 
OBJS += kissfft/kiss_fft.o kissfft/kiss_fftr.o

default: kubus

kubus: $(OBJS)
	$(CXX) -o kubus $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(FLAGS) $^

clean:
	rm -f $(OBJS) kubus
