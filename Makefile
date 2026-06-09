CC       = g++
CXXFLAGS = -I./include -std=c++23 -Wall -Wextra

SRCS    := $(wildcard src/*.cc)
OBJS    := $(patsubst src/%.cc, build/%.o, $(SRCS))
SCRIPTS := $(wildcard scripts/*)
TARGET   = build/client
PREFIX   = /usr/local
BIN_DIR  = $(PREFIX)/bin

$(shell mkdir -p build)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

build/%.o: src/%.cc
	$(CC) $(CXXFLAGS) -c $< -o $@

.PHONY: run install uninstall clean

run: $(TARGET)
		./$(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(BIN_DIR)/notes-client
	@for script in $(SCRIPTS); do \
		name=$$(basename $$script); \
		install -Dm755 $$script $(DESTDIR)$(BIN_DIR)/$${name%.*}; \
	done

uninstall:
	rm -f $(DESTDIR)$(BIN_DIR)/notes-client
	@for script in $(SCRIPTS); do \
		name=$$(basename $$script); \
		rm -f $(DESTDIR)$(BIN_DIR)/$${name%.*}; \
	done

clean:
	rm -rf build/*
