all: multiview

multiview: multiview.c
	gcc -Wall -O0 -g -o multiview `pkg-config --cflags --libs gstreamer-1.0` multiview.c

clean:
	rm multiview
