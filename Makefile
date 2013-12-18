all:
	ino build
clean:
	ino clean
install:
	ino upload
serial:
	ino serial

run: all install serial

