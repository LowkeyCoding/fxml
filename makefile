test:
	gcc tests.c ./lib/CuTest.c -o test

demo:
	gcc sxml_demo.c -o demo