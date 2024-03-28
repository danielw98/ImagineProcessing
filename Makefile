main=image_editor
build:
	gcc -o $(main) src/main.c src/helpers.c src/image.c src/commands.c src/filters.c src/histogram.c -Wall -Wextra -lm
noWarnings:
	gcc -o $(main) src/main.c src/helpers.c src/image.c src/commands.c src/filters.c src/histogram.c -lm
clean:
	-rm $(main) 2> /dev/null
