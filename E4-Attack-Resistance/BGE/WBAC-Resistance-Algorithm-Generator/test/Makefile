aes_test:
	clang++ -Wall -std=c++11 aes_test.cc ../src/aes/aes.cc -o aes_test
	./aes_test
	rm aes_test

wbmatrix_test:
	clang++ -Wall -std=c++11 ../libs/random/random.cc ../libs/wbmatrix/wbmatrix.cc wbmatrix_test.cc -o wbmatrix_test
	./wbmatrix_test
	rm wbmatrix_test

cejo_test:
	clang++ -Wall -std=c++11 ../libs/random/random.cc ../libs/wbmatrix/wbmatrix.cc ../src/aes/aes.cc ../src/cejo/cejo.cc cejo_test.cc -o cejo_test
	./cejo_test
	rm cejo_test

shiy_test:
	clang++ -Wall -std=c++11 ../libs/random/random.cc ../libs/wbmatrix/wbmatrix.cc ../src/aes/aes.cc ../src/cejo/cejo.cc ../src/shiy/shiy.cc shiy_test.cc -o shiy_test
	./shiy_test
	rm shiy_test

random_test:
	clang++ -Wall -std=c++11 ../libs/random/random.cc random_test.cc -o random_test
	./random_test
	rm random_test

test:
	clang test.c -o test