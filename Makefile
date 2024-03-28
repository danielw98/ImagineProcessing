main=image_editor
build:
	gcc -o $(main) src/main.c src/helpers.c src/image.c src/commands.c src/filters.c src/histogram.c -Wall -Wextra -lm
noWarnings:
	gcc -o $(main) src/main.c src/helpers.c src/image.c src/commands.c src/filters.c src/histogram.c -lm
runTests:
	gcc -o $(main) src/main.c src/helpers.c src/image.c src/commands.c src/filters.c src/histogram.c -Wall -Wextra -lm
	chmod u+x scripts/*
	chmod u+x run_tests.sh
	./run_tests.sh $(main)
clean:
	rm $(main)
