CXX=g++
INCLUDES=
# FLAGS=-D__MACOSX_CORE__ -c
FLAGS=-D__UNIX_JACK__ -c
#LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \ -framework IOKit -framework Carbon  -framework OpenGL \
#	-framework GLUT -framework Foundation \
#	-framework AppKit -lstdc++ -lm

LIBS= -ljack -lGL -lGLU -lglut -lstdc++ -lm -lpthread

OBJS= RtAudio.o kubus.o

default: kubus

kubus: $(OBJS)
	$(CXX) -o kubus $(OBJS) $(LIBS)

kubus.o: kubus.cpp RtAudio.h
	$(CXX) $(FLAGS) kubus.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f $(OBJ) kubus
