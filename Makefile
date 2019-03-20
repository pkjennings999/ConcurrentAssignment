convMake: conv-harness.c convTester.c hashmap.c iterator.c
	gcc -g -O3 -msse4 -fopenmp conv-harness.c convTester.c -o convTester.out