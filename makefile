madvise = madvise.c
syncmc = syncmc.c
CC = gcc
flag = -Wall --std=gnu99 -lpthread -lm


compile :
		$(CC) $(madvise) -o madvise $(flag)
		$(CC) $(syncmc) -o syncmc $(flag)




clean :
		rm syncmc madvise



#use perf command:
# https://millyz.github.io/ta/os3150_2016/mem1-lab/mem1/part1_2.html
