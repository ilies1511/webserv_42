NAME := webserv
.DEFAULT_GOAL := all
CPP := c++
AR := ar
RM := rm -rf

################################################################################
###############                  DIRECTORIES                      ##############
################################################################################

OBJ_DIR := _obj
# INC_DIRS := .
#INC_DIRS := Includes Includes/Extra Includes/Tests Includes/Config
# SRC_DIRS := .
#SRC_DIRS := src src/Extra src/Tests src/playground src/config

INC_DIRS := $(abspath	Includes \
			Includes/Extra \
			Includes/Tests \
			Includes/core \
			Includes/http \
			Includes/handler \
			Includes/utils \
			Includes/Config \
			Includes/StatusCodes \
			Includes/Request)
SRC_DIRS := $(abspath	src \
			src/Extra \
			src/Tests \
			src/playground \
			src/Config \
			src/core \
			src/http \
			src/handler \
			src/utils \
			src/Request)



# INC_DIRS := $(abspath Includes Includes/Extra Includes/Tests)
# SRC_DIRS := $(abspath src src/Extra src/Tests src/playground)
# Tell the Makefile where headers and source files are

vpath %.hpp $(INC_DIRS)
vpath %.cpp $(SRC_DIRS)

# there for preproc. -- BEGIN
HEADERS :=	Log.hpp \
			printer.hpp \
			test.hpp \
			playground.hpp \
			webserv.hpp \
			parser.hpp \
			route.hpp \
			serverConfig.hpp \
			token.hpp \
			validation.hpp \
			requestParsing.hpp \
			webserv.hpp \
			Request.hpp \
			Response.hpp \
			Connection.hpp \
			HTTP_Parser.hpp \
			Buffer.hpp

HDR_CHECK := $(addprefix $(OBJ_DIR)/, $(notdir $(HEADERS:.hpp=.hpp.gch)))
# there for preproc. -- END

################################################################################
###############                  SOURCE FILES                     ##############
################################################################################

MAIN_FILE := main.cpp


SRC_TEST_MAIN := main_tests.cpp

EXTRA_FILES := Log.cpp printer.cpp
EXTRA := $(addprefix Extra/, $(EXTRA_FILES))

TEST_FILES := test.cpp
TEST := $(addprefix Tests/, $(TEST_FILES))

DUMMY_REPO_FILES := dummy_file.cpp
DUMMY_REPO := $(addprefix dummy_repo/, $(DUMMY_REPO_FILES))

PLAYGROUND_REPO_FILES := play.cpp
PLAYGROUND_REPO := $(addprefix playground/, $(PLAYGROUND_REPO_FILES))

CORE_REPO_FILES := Server.cpp Connection.cpp utils_Connection.cpp
CORE_REPO := $(addprefix core/, $(CORE_REPO_FILES))

UTILS_REPO_FILES := utils_pollserver.cpp Buffer.cpp
UTILS_REPO := $(addprefix utils/, $(UTILS_REPO_FILES))

HTTP_REPO_FILES := Request.cpp Response.cpp HTTP_Parser.cpp
HTTP_REPO := $(addprefix http/, $(HTTP_REPO_FILES))

HANDLER_REPO_FILES := StaticFileHandler.cpp
HANDLER_REPO := $(addprefix handler/, $(HANDLER_REPO_FILES))

CONFIG_DIR_FILES := serverConfig.cpp \
					route.cpp \
					token.cpp \
					parser.cpp \
					validation.cpp
CONFIG_DIR := $(addprefix Config/, $(CONFIG_DIR_FILES))

REQUEST_DIR_FILES := requestParsing.cpp
REQUEST_DIR := $(addprefix Request/, $(REQUEST_DIR_FILES))

SRC := src_file.cpp



#Combines all
MELTING_POT :=	$(SRC) \
				$(EXTRA) \
				$(TEST) \
				$(PLAYGROUND_REPO) \
				$(CORE_REPO) \
				$(UTILS_REPO) \
				$(HTTP_REPO) \
				$(HANDLER_REPO) \
				$(REQUEST_DIR) \
				$(CONFIG_DIR)

# SRCS := $(MAIN_FILE) $(addprefix src/, $(SRC) $(EXTRA) $(TEST) $(PLAYGROUND_REPO) $(CORE_REPO))
SRCS := $(MAIN_FILE) $(addprefix src/, $(MELTING_POT))

OBJS := $(addprefix $(OBJ_DIR)/, $(SRCS:%.cpp=%.o))

################################################################################
########                         COMPILING                      ################
################################################################################

# CFLAGS := -Wall -Wextra -Werror -g -MMD -MP -I$(INC_DIRS)
CFLAGS :=	-Wall -Werror -Wextra -Wpedantic -Wshadow -Wno-shadow -std=c++17 \
			-Wconversion -Wsign-conversion -g -MMD -MP \
			$(addprefix -I, $(INC_DIRS))
CFLAGS_SAN := $(CFLAGS) -fsanitize=address
LDFLAGS := -lncurses
LDFLAGS_SAN := -lncurses -fsanitize=address
ARFLAGS := -rcs

# ANSI color codes
GREEN := \033[0;32m
MAGENTA := \033[0;35m
BOLD := \033[1m
NC := \033[0m # Reset

#Test/Playground exec.
NAME_TEST=tests.out

all: $(NAME)

# $(NAME): $(OBJS) $(HDR_CHECK)
$(NAME): $(OBJS)
#$(AR) $(ARFLAGS) $(NAME) $(OBJS)
	$(CPP) $(LDFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(BOLD)Successful Compilation$(NC)"

# Rule to compile .o files
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CPP) $(CFLAGS) -c $< -o $@

# Rule to create precompiled headers --> precom. specific
$(OBJ_DIR)/%.hpp.gch: %.hpp | $(OBJ_DIR)
	$(CPP) $(CFLAGS) -x c++-header -c $< -o $@

# Ensure the directories exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME) $(NAME_TEST) server client poll pollserver
	@echo "$(MAGENTA)$(BOLD)Executable + Object Files cleaned$(NC)"

re: fclean submodule_update all

submodule_update:
	git submodule update --remote --merge

bonus: all

san: fclean
	make CFLAGS="$(CFLAGS_SAN)" LDFLAGS="$(LDFLAGS_SAN)"
	@echo "$(GREEN)$(BOLD)Successful Compilation with fsan$(NC)"

re_sub: submodule_rebuild

submodule_rebuild:
	git submodule deinit -f .
	git submodule update --init --recursive

debug: clean
debug: CFLAGS += -DDEBUG
debug: $(NAME)

redebug: fclean debug

test:
	make $(NAME_TEST) MAIN_FILE="$(SRC_TEST_MAIN)" NAME=$(NAME_TEST)

retest: fclean test

server:
	$(CPP) -Wall -Werror -Wextra -o server src/playground/2_server.cpp

client:
	$(CPP) -Wall -Werror -Wextra -o client src/playground/3_client.cpp

poll:
	$(CPP) -Wall -Werror -Wextra -o poll src/playground/4_simple_poll.cpp

pollserver:
	$(CPP) -Wall -Werror -Wextra -o pollserver src/playground/5_pollserver.cpp

-include $(OBJS:%.o=%.d)

.PHONY: all clean fclean re bonus re_sub submodule_rebuild san debug test test_cases server client
