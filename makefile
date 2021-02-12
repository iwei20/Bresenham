name = bresenham
all: run

run: $(name).out
	./$(name).out
	-display $(name).ppm
	echo $(name).ppm

$(name).out: $(name).cpp
	g++ -o $(name).out $(name).cpp

clean:
	rm $(name).out
	rm $(name).ppm
