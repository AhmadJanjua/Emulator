CC := g++
CFLAGS := -Wall -Wextra -std=c++17 -I./include -I/usr/include/SDL2
LDFLAGS := -L/usr/lib/x86_64-linux-gnu/cmake/SDL2 -lSDL2
SRCDIR := src
OBJDIR := build
SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

all: $(OBJDIR) a.out

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

a.out: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJDIR) a.out
