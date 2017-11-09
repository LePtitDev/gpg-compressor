TARGET = main
BINDIR = bin
FLAGS = -std=c++11

define HEAD_FILES
	src/bitmap.h \
	src/process.h \
	src/huffman.h \
	src/filter.h \
	src/image.h \
	src/format/image-ppm.h
endef

define SRC_FILES
	src/main.cpp \
	src/process.cpp \
	src/format/image-ppm.cpp
endef

all: $(BINDIR) $(HEAD_FILES) $(SRC_FILES)
	g++ $(SRC_FILES) -o $(BINDIR)/$(TARGET) $(FLAGS)

$(BINDIR):
	mkdir "$(BINDIR)"