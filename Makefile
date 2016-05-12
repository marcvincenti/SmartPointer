SRCD = src
INCD = include
OBJD = obj
COMPILER = g++
OPTIONS = -ggdb -std=c++11 -O0
EXEC = prog

OBJFILES = pointeur.o main.o 
OBJS = $(OBJFILES:%.o=$(OBJD)/%.o)

default: initial $(EXEC)

initial:
	@mkdir -p obj

$(EXEC): $(OBJS)
	$(COMPILER) -o $@ $^ $(OPTIONS)

$(OBJD)/%.o: $(SRCD)/%.cpp
	$(COMPILER) -o $@ -c $< $(OPTIONS)

.PHONY: clean, mrproper

clean:
	@rm -f $(OBJD)/*.o

mrproper: clean
	@rm -f $(EXEC)
