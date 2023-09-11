UNAME := $(shell uname)

INCFLAGS = -I vecmath/include/

FRAMEWORKS = -framework Glut
FRAMEWORKS += -framework OpenGL

LINKFLAGS += -L vecmath/lib/ -lvecmath

CFLAGS    = -O2 -Wall -ansi -DSOLN
CC        = g++
SRCS      = main.cpp
OBJS      = $(SRCS:.cpp=.o)
PROG      = assignment0

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(FRAMEWORKS) $(CFLAGS) $(OBJS) -o $@ $(INCFLAGS) $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm $(OBJS) $(PROG)
