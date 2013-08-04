BIN = Snake

CXX = g++
BTYPE = -g3 -ggdb3 -O1
CXXFLAGS = -std=gnu++11 -Wall -DGLEW_STATIC -include GL/glew.h ${BTYPE}
LIBS = -lGL -lGLU -lGLEW -lglfw3 -lXi -lXrandr -lSOIL

OBJ_DIR = obj
SRC = point.cpp timer.cpp shaderprogram.cpp texture.cpp tile.cpp map.cpp game.cpp main.cpp
OBJ = ${SRC:%.cpp=${OBJ_DIR}/%.o}

.PHONY: all clean

all: ${BIN}
clean:
	${RM} ${OBJ_DIR}/*.o
	${RM} ${BIN}

${BIN}: ${OBJ_DIR} ${OBJ}
	@echo "LD 	$@"
	@${CXX} -o $@ ${OBJ} ${LIBS}

${OBJ_DIR}/%.o: %.cpp
	@echo "CXX 	$<"
	@${CXX} -c ${CXXFLAGS} -o $@ $<

${OBJ_DIR}:
	@mkdir -p ${OBJ_DIR}
