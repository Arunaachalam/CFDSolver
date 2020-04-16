CC  = gcc -g3 -DNDEBUG
CXX = g++ -g3 -DNDEBUG
FC  = gfortran
LINKER = $(CXX)

ANSI_CFLAGS  = -ansi
ANSI_CFLAGS += -std=c++1z
ANSI_CFLAGS += -pedantic
ANSI_CFLAGS += -Wextra

CFLAGS   = -O3 -Wno-format  -Wall $(ANSI_CFLAGS)
CXXFLAGS = $(CFLAGS)
FCFLAGS  = 
CPPFLAGS = -std=c++1z
LFLAGS   =  
DEFINES  = -D_GNU_SOURCE
INCLUDES =
LIBS     =
