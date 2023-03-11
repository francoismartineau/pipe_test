NAME = pipe_test

all: $(NAME)

$(NAME): main.c
	gcc $^ -o $(NAME)

clean:
	rm -f $(NAME)

.PHONY: all clean