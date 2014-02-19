CC = g++-4.8
FLAGS = -Wall -Werror
LIBRARIES = -fopenmp
TARGET = $(shell basename `pwd`)

ifeq ($(DEBUG), 1)
	override FLAGS += -O0 -g
endif

SRC_DIR = src
OBJECTS_DIR = obj

INPUTS = $(wildcard $(SRC_DIR)/*.cc)
INPUTS_TMP = $(subst $(SRC_DIR),$(OBJECTS_DIR),$(INPUTS))
OBJECTS = $(INPUTS_TMP:%.cc=%.o)
DEPFILES = $(OBJECTS:%.o=%.d)


# main application
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(LIBRARIES) -o $@ $(OBJECTS)

$(OBJECTS_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJECTS_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -c -o $@ $<

$(OBJECTS_DIR)/%.d: $(SRC_DIR)/%.cc | $(OBJECTS_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -M -MF $@ -MT $(@:%.d=%.o) $<

$(OBJECTS_DIR):
	mkdir $(OBJECTS_DIR)


# running, testing, etc

run: all
	./$(TARGET)

-include $(DEPFILES)


# cleanup

.PHONY: clean
clean:
	-rm -rf $(OBJECTS_DIR) $(TARGET)

