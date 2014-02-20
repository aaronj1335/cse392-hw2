CC = g++
FLAGS = -Wall -Werror
LIBRARIES = -fopenmp
TARGET = $(shell basename `pwd`)

ifeq ($(DEBUG), 1)
	override FLAGS += -O0 -g
endif

ifeq ($(shell uname), Darwin)
	override CC = g++-4.8
endif

SRC_DIR = src
OBJ_DIR = obj
VAR_DIR = var
RESULTS_DIR = results

INPUTS = $(wildcard $(SRC_DIR)/*.cc)
INPUTS_TMP = $(subst $(SRC_DIR),$(OBJ_DIR),$(INPUTS))
OBJECTS = $(INPUTS_TMP:%.cc=%.o)
DEPFILES = $(OBJECTS:%.o=%.d)

PERF_FILES = $(VAR_DIR)/1d.txt $(VAR_DIR)/4d.txt

# main application
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(LIBRARIES) -o $@ $(OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -c -o $@ $<

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -M -MF $@ -MT $(@:%.d=%.o) $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)


# running, testing, etc

run: all
	./$(TARGET)

test: all
	@test/test

$(VAR_DIR):
	mkdir $@

$(VAR_DIR)/1d.txt: $(VAR_DIR)
	@python test/perfdata.py 1 > $@

$(VAR_DIR)/4d.txt: $(VAR_DIR)
	@python test/perfdata.py 4 > $@

perfdata: $(PERF_FILES)

$(RESULTS_DIR):
	mkdir $@

perf: perfdata all | $(RESULTS_DIR)
	cat var/1d.txt | ./$(TARGET) -n 3>$(RESULTS_DIR)/1d.txt 1>/dev/null
	cat var/4d.txt | ./$(TARGET) -nd 4 3>$(RESULTS_DIR)/4d.txt 1>/dev/null

-include $(DEPFILES)


# cleanup

.PHONY: clean
clean:
	-rm -rf $(OBJ_DIR) $(VAR_DIR) $(TARGET)

