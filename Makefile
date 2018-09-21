CC = g++
SRCS = random_bg_gen.cpp
PROG = random_bg_gen

OPENCV = `pkg-config opencv --cflags --libs`
LCURL = -lcurl 
LIBS = $(OPENCV) $(LCURL)
	
$(PROG):$(SRCS)
		  $(CC) -o $(PROG) $(SRCS) $(LIBS)

clean : 
	rm $(SRCS)
	rm $(PROG)
