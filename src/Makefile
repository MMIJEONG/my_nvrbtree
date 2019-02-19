all : trc_gen nv_rbtree 

trc_gen: trc_gen.c
	gcc -o trc_gen trc_gen.c

nv_rbtree: main.o nv_rbtree.o
	gcc -o nv_rbtree main.o  nv_rbtree.o

main.o : nv_rbtree.h main.c
	gcc -c -g main.c
nv_rbtree.o : nv_rbtree.c
	gcc -c -g nv_rbtree.c

clean :
	rm *.o nv_rbtree trc_gen

