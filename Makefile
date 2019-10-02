CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
STRIP = $(CROSS_COMPILE)strip

SYSROOT     := $(shell $(CC) --print-sysroot)
SDL_CFLAGS  := $(shell $(SYSROOT)/usr/bin/sdl-config --cflags)
SDL_LIBS    := $(shell $(SYSROOT)/usr/bin/sdl-config --libs)

CFLAGS = -ggdb -DTARGET_BITTBOY -DTARGET=$(TARGET) -D__BUILDTIME__="$(BUILDTIME)" -DLOG_LEVEL=3 -g3 $(SDL_CFLAGS) #-I$(CHAINPREFIX)/usr/include/ -I$(SYSROOT)/usr/include/  -I$(SYSROOT)/usr/include/SDL/ # -mhard-float
CXXFLAGS = $(CFLAGS)
LDFLAGS = $(SDL_LIBS) -lfreetype -lSDL -lSDL_image -lSDL_ttf

pc:
	gcc miyoo_setup.c -g -o miyoo_setup -ggdb -O0 -DDEBUG -lSDL_image -lSDL -lSDL_ttf -I/usr/include/SDL

miyoo:
	$(CXX) $(CFLAGS) $(LDFLAGS) miyoo_setup.c -o miyoo_setup.elf -DMIYOO

clean:
	rm -rf miyoo_setup.elf miyoo_setup
