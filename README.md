# ParaCL
A pet-project for the development of a programming language
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++ && cmake --build build

./build/ParaCL tests/test_1.dat

It is also possible to run the script: bash run_tests.sh, which will run all my tests