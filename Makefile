all: solver
	g++ solver.cpp -o solver
	
run: all
	./solver $(args)



