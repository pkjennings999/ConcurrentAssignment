convMake: conv-harness.c
	gcc -g -O3 -msse4 -fopenmp conv-harness.c -o a.out