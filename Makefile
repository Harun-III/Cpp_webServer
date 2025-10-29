NAME		=	webserv

# Specified Path For The Strict Rules
SRCS		=	$(shell find src -name "*.cpp")

OBJS_DIR	=	.objects/
OBJS		=	$(addprefix $(OBJS_DIR), $(SRCS:.cpp=.o))

# Specified Path For The Strict Rules
HEADERS		=	$(shell find src -name "*.hpp")

CC			=	c++
FLAGS		=	-Wall -Wextra -Werror -std=c++98
RM			=	rm -fr

INCLUDES	=	-I src/config -I src/http -I src/server -I src/cgi -I src/core -I src/tools

GREEN		=	"\033[1;32m"
YELOW		=	"\033[1;33m"
REDCL		=	"\033[1;31m"
RESET		=	"\033[0m"

all: start $(NAME) finish

start:
	@printf "\n"
	@echo $(GREEN)"Starting build..."
	@sleep 1
	@printf "Loading [ "

finish:
	@echo $(GREEN) ] 100%$(RESET)
	@echo $(GREEN)Project built.$(RESET)
	@printf "\n"

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@
	@printf $(GREEN)"."$(RESET)

clean:
	@$(RM) $(OBJS_DIR)
	@echo $(YELOW)Cleaning up 🧹💫$(RESET)

fclean: clean
	@$(RM) $(NAME)
	@echo $(REDCL)Purging all files 🗑️$(RESET)

re: fclean all

.PHONY: start clean finish
