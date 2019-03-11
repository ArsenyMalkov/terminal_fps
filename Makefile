LIBS = -lncursesw
CC = g++
CFLAGS = -Wall

fps: fps.cc
	$(CC) fps.cc $(CFLAGS) $(LIBS)

clean:
	-rm -f *.out
