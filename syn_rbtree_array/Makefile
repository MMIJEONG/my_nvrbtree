nvrbtree : main.o nv_rbtree.o
	gcc -o nvrbtree main.o  nv_rbtree.o -lm
main.o : nv_rbtree.h main.c
	gcc -c -o main.o main.c -lm
nv_rbtree.o : nv_rbtree.c
	gcc -c -o nv_rbtree.o nv_rbtree.c -lm

clean :
	rm *.o nvrbtree

