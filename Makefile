# Directories
SRCDIR		:= src/
OBJDIR		:= obj/
TEXDIR		:= textures/
SOUNDDIR	:= sounds/
SOILDIR		:= $(SRCDIR)SOIL/
SHADERDIR	:= shaders/
DOCDIR		:= doc/
DOXYGENDIR	:= $(DOCDIR)doxygen/
UTILDIR		:= util/
BINDIR		:= bin/
CONFIGDIR	:= config/

# Sources
SRCS := $(wildcard  $(SRCDIR)*.cpp)
SRCS += $(wildcard $(SOILDIR)*.c  )

# Compiler / Linker Configuration
CFLAGS		:= -std=c++11 -pthread -Wall -O3 -fno-strict-aliasing
CFLAGS_SOIL	:= -w
CFLAGS_CROSS	:= -std=c++11 -pthread -w -DWIN32 -O3
CFLAGS_EMS	:= -std=c++11 -Wall
LIBS		:= -lm -lGL -lGLU -lglut -lGLEW -lopenal -lvorbisfile

# Cross Compilation (Windows)
CROSSCC		:= i686-w64-mingw32-g++
CROSSLIBS	:= -lopengl32 -lglu32 -lglut -lm -lglew32 -lOpenAL32 -lvorbisfile

# Generic makefile:
OBJS		:= $(addprefix $(OBJDIR),$(notdir $(SRCS:.cpp=.o)))
OBJS		:= $(OBJS:.c=.o)
DEPS		:= $(addprefix $(OBJDIR),$(notdir $(SRCS:.cpp=.d)))
DEPS		:= $(DEPS:.c=.d)
TARGET 		:= planether
CROSSTARGET	:= $(TARGET).exe

all: $(OBJDIR) $(BINDIR) update_starter $(TARGET)
	@echo Compilation succesful

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $^ -o $(BINDIR)$(TARGET) $(LIBS)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

update_starter:
	$(UTILDIR)update_starter.sh

# SOIL Compilation (C)
$(OBJDIR)%.o: $(SOILDIR)%.c
	$(CC) $(CFLAGS_SOIL) -MMD -MP -c $< -o $@

run: all
	./$(BINDIR)$(TARGET)

valgrind: all
	valgrind ./$(BINDIR)$(TARGET)

.PHONY: clean

# Documentation (Doxygen)
doxygen:
	doxygen doc/Doxyfile

# Cross Compilation
cross: $(SRCS)
	mkdir -p win32
	$(CROSSCC) -m32 $(CFLAGS_CROSS) $^ -o win32/$(CROSSTARGET) $(CROSSLIBS)
	cp /usr/i686-w64-mingw32/bin/libgcc_s_sjlj-1.dll win32/libgcc_s_sjlj-1.dll
	cp /usr/i686-w64-mingw32/bin/libwinpthread-1.dll win32/libwinpthread-1.dll
	cp /usr/i686-w64-mingw32/bin/glew32.dll win32/glew32.dll
	cp /usr/i686-w64-mingw32/bin/libglut-0.dll win32/libglut-0.dll
	cp /usr/i686-w64-mingw32/bin/libstdc++-6.dll win32/libstdc++-6.dll
	cp /usr/i686-w64-mingw32/bin/OpenAL32.dll win32/OpenAL32.dll
	cp /usr/i686-w64-mingw32/bin/libvorbis-0.dll win32/libvorbis-0.dll
	cp /usr/i686-w64-mingw32/bin/libogg-0.dll win32/libogg-0.dll
	cp /usr/i686-w64-mingw32/bin/libvorbisfile-3.dll win32/libvorbisfile-3.dll

	cp -R $(SHADERDIR) win32/
	cp -R $(TEXDIR) win32/
	cp -R $(SOUNDDIR) win32/
	cp -R $(CONFIGDIR) win32/

# $(RM) is rm -f by default
clean:
	$(RM) *~ src/*~
	$(RM) -r $(BINDIR)
	$(RM) -r win32
	$(RM) -r $(OBJDIR)
	$(RM) -r $(DOXYGENDIR)
	$(RM) $(SHADERDIR)*/*~
	$(RM) $(SHADERDIR)*~
	$(RM) $(CONFIGDIR)*~
	$(RM) $(DOCDIR)*~
	$(RM) util/*~
	$(RM) $(SOUNDDIR)*~
	$(RM) $(TEXDIR)*~

-include $(DEPS)
