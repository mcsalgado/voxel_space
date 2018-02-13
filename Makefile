CXX = cl
CXXFLAGS = -nologo -std:c++14 -FC -WX -W4 -wd4100 -wd4201 -GR- -EHa- -MTd -Gm- -Fm
LFLAGS = -incremental:no -opt:ref
LIBS = user32.lib gdi32.lib winmm.lib

CXXFLAGS += -Od -Z7
CXXFLAGS += -Oi -O2

SRCS = $(filter-out src/precompiled.cpp, $(wildcard src/*.cpp))
OBJS = $(SRCS:src/%.cpp=build/%.obj)

RM = cmd.exe /C del

all: bin/voxel_space.exe bin/test_render.exe

bin/voxel_space.exe: $(OBJS)
	$(CXX) /Fo$(@D)/ /Fe$(@) /Fd$(@D)/ $(CXXFLAGS) build/precompiled.obj $(OBJS) /link $(LFLAGS) $(LIBS)

bin/test_render.exe: test/test_render.cpp build/render.obj build/map.obj
	$(CXX) /Fe$(@) /Fd$(@D)/ $(CXXFLAGS) test/test_render.cpp build/precompiled.obj build/render.obj build/map.obj /link $(LFLAGS)

build/%.obj: src/%.cpp src/%.h build/precompiled.h.gch
	$(CXX) /c /Yu"precompiled.h" /Fp"build/precompiled.h.gch" /Fo$(@D)/ /Fd$(@D)/ $(CXXFLAGS) $<

build/precompiled.h.gch: src/precompiled.cpp src/precompiled.h src/math.h src/map.h src/render.h src/input.h src/voxel_space.h
	$(CXX) /c /Yc"precompiled.h" /Fp$(@) /Fo"build/precompiled.obj" $(CXXFLAGS) $<

clean:
	$(RM) bin\voxel_space.exe $(subst /,\,$(OBJS)) bin\test_render.exe build\precompiled.h.gch

.PHONY: all clean
