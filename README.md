
# 2020.11.29, Seunghoon
please add camera.ko, textlcd.ko, segment.ko

textlcd function added
	-you can check which mode is activated
	-you can also check color number when you are activating r,g,b mod(1~10)
	
segment function added
	-you can check color number of r,g,b(1~10)
	-but I still don't know why 0 is not plotted on it.
	-Please, use the device driver which is loaded on teh SV210 board or
	use the c file which is included in the segment folder in the palette folder.

dotmatrix function added
	-you can check detected face number
	-use device driver on the board

gray scale mode added
	-you can see the video and real-time gray-scaled viedo.
	-but I don't knwo how gray scale function work exactily yet.
	-you can exit this mode when you press 'q'.

face detect mod changed
	-When you press 'q', you can exit face detect mode


# 2020.11.29, Jinho
