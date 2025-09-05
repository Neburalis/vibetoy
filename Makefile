CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Ilib
SRCDIR = src
GAMESDIR = $(SRCDIR)/games
OBJDIR = obj
GAMESOBJDIR = $(OBJDIR)/games

# Source files
CPP_SOURCES = $(wildcard $(SRCDIR)/*.cpp)
C_SOURCES = $(wildcard $(SRCDIR)/*.c)
GAMES_C_SOURCES = $(wildcard $(GAMESDIR)/*.c)

# Object files
CPP_OBJECTS = $(CPP_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
C_OBJECTS = $(C_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
GAMES_C_OBJECTS = $(GAMES_C_SOURCES:$(GAMESDIR)/%.c=$(GAMESOBJDIR)/%.o)

ALL_OBJECTS = $(CPP_OBJECTS) $(C_OBJECTS) $(GAMES_C_OBJECTS)
TARGET = tictactoe

all: $(TARGET)

$(TARGET): $(ALL_OBJECTS) | $(OBJDIR) $(GAMESOBJDIR)
	$(CXX) $(ALL_OBJECTS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(GAMESOBJDIR)/%.o: $(GAMESDIR)/%.c | $(GAMESOBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(GAMESOBJDIR):
	mkdir -p $(GAMESOBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean