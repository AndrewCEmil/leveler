CC = clang++
CFLAGS = -pipe -fomit-frame-pointer -ggdb -std=c++11 -stdlib=libc++

images: EasyBMP.o level.o
	$(CC) $(CFLAGS) -g EasyBMP.o level.o -o level

EasyBMP.o: EasyBMP.cpp EasyBMP*.h
	$(CC) $(CFLAGS) -c EasyBMP.cpp

level.o: level.cpp
	$(CC) $(CFLAGS) -c level.cpp

clean:
	rm -f *.o
	rm level
	rm img*.bmp
