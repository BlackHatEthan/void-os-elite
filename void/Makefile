# Void OS Elite - Makefile
# Native C/ASM Shell Environment

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -g -lm
LDFLAGS = -ldl -lm
STATIC_LDFLAGS = -static -ldl -lm
INCLUDES = -Iinclude -Isrc
SRCDIR = src
BUILDDIR = build
SOURCES = $(shell find $(SRCDIR) -name "*.c")
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET = void

# Colors for output
GREEN = \033[0;32m
BLUE = \033[0;34m
RESET = \033[0m

.PHONY: all clean install static portable

all: $(TARGET)
	@echo "$(GREEN)Void OS Elite compiled successfully.$(RESET)"

$(TARGET): $(OBJECTS)
	@echo "$(BLUE)Linking...$(RESET)"
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "$(GREEN)Build complete.$(RESET)"

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(BLUE)Compiling $<...$(RESET)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(BLUE)Cleaning build artifacts...$(RESET)"
	rm -rf $(BUILDDIR) $(TARGET)
	@echo "$(GREEN)Clean complete.$(RESET)"

install: $(TARGET)
	@echo "$(BLUE)Installing Void OS Elite...$(RESET)"
	cp $(TARGET) /usr/local/bin/void
	@echo "$(GREEN)Installation complete. Run 'void' to start.$(RESET)"

# Development helpers
run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -DDEBUG -g3
debug: $(TARGET)

static: LDFLAGS = $(STATIC_LDFLAGS)
static: $(TARGET)
	@echo "$(GREEN)Static binary created: $(TARGET)$(RESET)"
	@if [ "$(shell uname)" = "Linux" ]; then \
		echo "$(BLUE)This binary can run on any Linux system without dependencies.$(RESET)"; \
	else \
		echo "$(BLUE)Note: Static linking on macOS is limited. For Linux, compile on Linux system.$(RESET)"; \
	fi

portable: static
	@echo "$(BLUE)Creating portable package...$(RESET)"
	@mkdir -p portable
	@cp $(TARGET) portable/void
	@echo "#!/bin/bash" > portable/run.sh
	@echo "# Portable Void OS Elite" >> portable/run.sh
	@echo "cd \"\$$(dirname \"\$$0\")\"" >> portable/run.sh
	@echo "./void" >> portable/run.sh
	@chmod +x portable/run.sh
	@echo "$(GREEN)Portable package created in portable/ directory$(RESET)"
	@echo "$(BLUE)Copy the 'portable' directory to USB and run ./run.sh$(RESET)"

