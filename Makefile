NAME        = webserver

COMPILER    = c++

CFLAGS      = -Wall -Werror -Wextra -std=c++98 -Wshadow
DEBUG_FLAGS = -DDEBUG -g

SRC_DIR     = src/
OBJ_DIR     = obj/
INCLUDE_DIR = inc/

PARSER_DIR   = parser/
SERVER_DIR  = server/
UTILS_DIR   = utils/

PARSER_FILES =   ServerConfig.cpp \
                ConfigParser.cpp \
                LocationConfig.cpp

SERVER_FILES = Server.cpp 

UTILS_FILES = Utils.cpp \
                Debug.cpp

MAIN_FILE   = main.cpp

SOURCE_FILES = $(addprefix $(PARSER_DIR), $(PARSER_FILES)) \
               $(addprefix $(SERVER_DIR), $(SERVER_FILES)) \
               $(addprefix $(UTILS_DIR), $(UTILS_FILES)) \
               $(MAIN_FILE)

SRCS        = $(addprefix $(SRC_DIR), $(SOURCE_FILES))
OBJS        = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRCS))
DEPS        = $(OBJS:.o=.d)

INCLUDES    = -I $(INCLUDE_DIR) \
              -I $(INCLUDE_DIR)$(SERVER_DIR) \
              -I $(INCLUDE_DIR)$(PARSER_DIR) \
              -I $(INCLUDE_DIR)$(UTILS_DIR)

all: $(NAME)

$(NAME): $(OBJS)
	$(COMPILER) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "\033[1;36m"
	@echo "==================================================="
	@echo "\t🍏​🍎 $(NAME) Compiled! 🍎​🍏"
	@echo "==================================================="
	@echo "\033[0m"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(@D)
	$(COMPILER) $(CFLAGS) $(INCLUDES) -MMD -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	@echo "\033[1;33m"
	@echo "==================================================="
	@echo "\t🧹 Object files cleaned! 🧹"
	@echo "==================================================="
	@echo "\033[0m"

fclean: clean
	rm -f $(NAME)
	@echo "\033[1;34m"
	@echo "==================================================="
	@echo "\t🗑️  Executable removed! 🗑️"
	@echo "==================================================="
	@echo "\033[0m"

re: fclean all

run: all
	@echo "\033[1;32m"
	@echo "==================================================="
	@echo " ⚰️​  Executing $(NAME)... ⚰️​​​​"
	@echo "==================================================="
	@echo "\033[0m"
	./$(NAME)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: fclean all
	@echo "\033[1;31m"
	@echo "==================================================="
	@echo "\t🐛 DEBUG MODE ENABLED 🐛"
	@echo "==================================================="
	@echo "\033[0m"

#		To show debug info you should do somenthing like this:
#				#ifdef DEBUG
#                   std::cout << "debug message" << std::endl;
#               #endif

-include $(DEPS)
.PHONY: all clean fclean re run debug
