
# 2020.11.29, Seunghoon
please add camera.ko, textlcd.ko, segment.ko

## textlcd function added
	-you can check which mode is activated
	-you can also check color number when you are activating r,g,b mod(1~10)
	
## segment function added
	- you can check color number of r,g,b(1~10)
	- but I still don't know why 0 is not plotted on it.
	- Please, use the device driver which is loaded on the board or
      use the c file which is included in the segment folder in the palette folder.

## dotmatrix function added
	-you can check detected face number
	-use device driver on the board

## gray scale mode added
	- you can see the video and real-time gray-scaled video.
	- but I don't know how gray scale function work exactly yet.
	- you can exit this mode when you press 'q'.
## face detect mod changed
    - When you press 'q', you can exit face detect mode


# 2020.11.29, Jinho
## Start
    - move README.md to project root dir
    - add redeclaration guards to 
        - textlcd
        - segment
        - dotmatrix
### what is a redeclaration guards?
```C
// camera.h
#ifndef TEXTLCD_H
#define TEXTLCD_H

void do_something();

#endif
```
When the code is compiled, the preprocessor checks whether
HEADERFILE_H has been previously defined. If this is the first time we
have included the header, HEADERFILE_H will not have been
defined. Consequently, the compiler defines HEADERFILE_H and includes
the contents of the file.

If the header is included again into the same file, HEADERFILE_H will
already have been defined from the first time that the contents of the
header were included; the ifndef guard will then ensure that the
contents of the header will be ignored.

These header guards prevent redeclaration of any identifiers such as
types, enums, classes, and static variables. They also prevent
recursive inclusions; for example, a case where “file1.h” includes
“file2.h” and “file2.h” includes “file1.h”.
    - clean up indentation 

## modularize facedetect
    - we facedetect module should not have touchlcd or camera module if it needs, it should get as a parameter
    - init_facdetect.c: remove other device driver opening part from facedetect.c
## implement camera part
    - c : start camera
    - s : save image 
    - S : load saved image and start editing
## implement modularized facedetection 
## implement modularized grayscaling 

# 2020.12.01~03, Jinho
# modularize gpio button
# modularize keypad 
# make keypad non blocking
