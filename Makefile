CLD_HOME=../clibdocker
PROGRAMS = v-rex
SRC_DIR=./src
OUT_DIR=./bin/release
TEST_DIR=./test
OBJ_DIR=./obj
MKDIR_P=mkdir -p

CFLAGS=-std=c11 `pkg-config --cflags json-c` `pkg-config --cflags libcurl` -I/usr/Motif21/include -I/usr/X11R6/include  -I$(CLD_HOME)/src
LIBS=$(CLD_HOME)/obj/docker_*.o $(CLD_HOME)/obj/log.o `pkg-config --libs json-c` `pkg-config --libs libcurl` -L/usr/Motif21/lib -L/usr/X11R6/lib -lXbae -lXm -lXt -lSM -lICE -lX11 -lpthread
debug: CFLAGS += -DDEBUG -g -O0
debug: OUT_DIR = ./bin/debug

.PHONY: directories

# see https://www.gnu.org/software/make/manual/html_node/Wildcard-Function.html
# Get list of object files, with paths
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))

all: 		directories $(PROGRAMS)
debug: 		directories $(PROGRAMS)

## see https://stackoverflow.com/questions/1950926/create-directories-using-make-file
## for creating output directories
directories: $(OUT_DIR) $(OBJ_DIR)

$(OUT_DIR):
			$(MKDIR_P) $(OUT_DIR)

$(OBJ_DIR):
			$(MKDIR_P) $(OBJ_DIR)

v-rex: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OUT_DIR)/$@ $^ $(LIBS)
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	-rm -f ./bin/release/* ./bin/debug/* $(OBJ_DIR)/*.o
