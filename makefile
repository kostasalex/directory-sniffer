EXECUTABLE = sniffer

BUILD = build
SRC = src
INC = include


SOURCE = $(wildcard $(SRC)/*.cpp)
	
OBJECT = $(patsubst %,$(BUILD)/%, $(notdir $(SOURCE:.cpp=.o)))

CC = g++
CFLAGS = -Wall -g -I$(INC)

GREEN = \033[1;32m
BLUE = \033[1;34m
YELLOW = \033[1;33m
NC = \033[1;0m
	
	
$(EXECUTABLE) : $(OBJECT)
	@echo "$ Compiling...$(NC)"
	$(CC) -o $@ $^
	
$(BUILD)/%.o : $(SRC)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

	
clean: 
	@echo "$(BLUE)🧹 Cleaning..$(NC)"
	rm -f $(BUILD)/* $(EXECUTABLE)

cleanf: 
	@echo "$(BLUE)🧹 Cleaning files..$(NC)"
	rm -f files/*.out 