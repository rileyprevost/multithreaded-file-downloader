FLAGS = -Wall -fsanitize=address,undefined -lcurl
OBJS = download.c
TARGET = download

all: download

download: ${OBJS}
	gcc ${FLAGS} -g ${OBJS} -o ${TARGET}