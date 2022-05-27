cw_2_2: main.o png_work.o picture_work.o CW_tasks.o
	gcc main.o png_work.o picture_work.o CW_tasks.o -lpng -o cw_2_2

main.o: main.c png_work.h picture_work.h CW_tasks.h
	gcc -c main.c

png_work.o: png_work.c png_work.h
	gcc -c png_work.c

picture_work.o: picture_work.c picture_work.h png_work.h
	gcc -c picture_work.c

CW_tasks.o: CW_tasks.c picture_work.h png_work.h
	gcc -c CW_tasks.c

clean:
	rm *.o cw_2_2

