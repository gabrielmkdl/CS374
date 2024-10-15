To compile the code type in "gcc -g --std=gnu99 -o movies assignment1.c"
This will create an executable file named movies in which to run the program type "./movies fileName" where the fileName is whatever name the file of all the movies is under
To run with gdb type in "gdb ./movies" then type in "r movies_sample_1.csv"
To run with valgrind with all its flags type in "valgrind --leak-check=full --track-origins=yes ./movies movies_sample_1.csv"
