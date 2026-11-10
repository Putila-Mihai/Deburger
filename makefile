# -std=gnu11  C11 plus GNU extensions
# -g          debug symbols
# -O0         no optimization
# -Wall -Wextra   enable most useful warnings
CFLAGS ?= -std=gnu11 -g -O0 -Wall -Wextra

SRC    := $(wildcard src/*.c)
OBJ    := $(SRC:src/%.c=build/%.o)

# Link rule. First rule in the file is the default, so plain `make` builds this.
# $@ = the target (deburger), $^ = all prerequisites (every .o file).
deburger: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# $< = the first prerequisite (the .c file). -c = compile only, don't link.
# -MMD also writes build/X.d, listing which headers X.c includes.
build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

build:
	mkdir -p build

# Load the .d files so editing a header rebuilds every .c file that includes it.
# The leading "-" ignores errors for missing files (none exist on the first build).
-include $(OBJ:.o=.d)

clean:
	rm -rf build deburger

# Tells make "clean" is an action, not a file, so it runs even if a file
# named "clean" exists.
.PHONY: clean