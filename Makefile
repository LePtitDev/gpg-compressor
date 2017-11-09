EXECUTABLE = build/main
FLAGS = -std=c++11

define HEAD_FILES
	src/bitmap.h \
	src/process.h \
	src/filter.h \
	src/image.h \
	src/format/image-ppm.h
endef

define SRC_FILES
	src/main.cpp \
	src/process.cpp \
	src/format/image-ppm.cpp
endef

all: $(HEAD_FILES) $(SRC_FILES)
	g++ $(SRC_FILES) -o $(EXECUTABLE) $(FLAGS)