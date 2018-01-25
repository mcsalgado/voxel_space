CXX = cl
CXXFLAGS = -nologo -std:c++14 -FC -WX -W4 -wd4100 -wd4201 -GR- -EHa- -MTd -Gm- -Fm
LFLAGS = -incremental:no -opt:ref
LIBS = user32.lib gdi32.lib winmm.lib

CXXFLAGS += -Od -Z7
CXXFLAGS += -Oi -O2

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:src/%.cpp=build/%.obj)

all: bin/voxel_space.exe bin/test_render.exe

bin/voxel_space.exe: $(OBJS)
	$(CXX) /Fo$(@D)/ /Fe$(@) /Fd$(@D)/ $(CXXFLAGS) $(OBJS) /link $(LFLAGS) $(LIBS)

bin/test_render.exe: test/test_render.cpp build/render.obj build/map.obj
	$(CXX) /Fo$(@D)/ /Fe$(@) /Fd$(@D)/ $(CXXFLAGS) test/test_render.cpp build/render.obj build/map.obj /link $(LFLAGS)

build/%.obj: src/%.cpp src/%.h
	$(CXX) /c /Fo$(@D)/ /Fe$(@) /Fd$(@D)/ $(CXXFLAGS) $<

clean:
	$(RM) bin/voxel_space.exe $(OBJS) bin/test_render.exe

.PHONY: all clean
