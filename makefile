all:  check_difference uks_curl main
	
uks_curl: uks_curl.cpp uks_curl.o
	g++ -c uks_curl.cpp
check_difference: check_difference.cpp check_difference.o
	g++ -c check_difference.cpp
main: check_difference.o uks_curl.o ./JsonBox/build/objs/*.o
	g++ -o main check_difference.o uks_curl.o ./JsonBox/build/objs/*.o -lcurl
clean:
	rm -f *.o main
