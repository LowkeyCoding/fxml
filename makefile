test:
	gcc tests.c ./libs/CuTest.c -o test

demo:
	gcc sxml_demo.c -o demo