
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "textlcd.h"


int text_dev;
struct strcommand_varible strcommand;


void init_textlcd(){
        strcommand.rows = 0;            
        strcommand.nfonts = 0;          
        strcommand.display_enable = 1;          
        strcommand.cursor_enable = 0;           
        strcommand.nblink = 0;          
        strcommand.set_screen = 0;
        strcommand.set_rightshit = 1;           
        strcommand.increase = 1;
        strcommand.nshift = 0;          
        strcommand.pos = 10;            
        strcommand.command = 1;         
        strcommand.strlength = 16;
	text_dev = open("/dev/textlcd", O_WRONLY|O_NDELAY );
	if(text_dev == -1){
		printf("textlcd device driver not open!!");
	}
}

void textlcd_write(char ch, unsigned short brush_size, unsigned short brush_color){
    char brush[16]={' ',};
	int i =0, j= 0;
	unsigned short tmp_red,tmp_green,tmp_blue;
    switch(ch){
    case 'w':	
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
		write(text_dev,"WHITE           ",16);
		strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
        break;
    case 'r':
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
        write(text_dev,"Red             ",16);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
		tmp_red = (brush_color>>10)/6;
        sprintf(brush,"      %d",(brush_color>>10)/6);
		for(i = 0 ;brush[i] != '\0';i++){

        }
        for(j = i;j<16;j++){
            brush[j] = ' ';
        }
        write(text_dev,brush,16);
        strcommand.pos = 0;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);

		break;
    case 'b':
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
        write(text_dev,"Blue            ",16);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
		tmp_blue = brush_color/6;
        sprintf(brush,"      %d", tmp_blue);
        for(i = 0 ;brush[i] != '\0';i++){

        }
        for(j = i;j<16;j++){
            brush[j] = ' ';
        }
        write(text_dev,brush,16);
        strcommand.pos = 0;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);

        break;
	case 'g':       
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
        write(text_dev,"Green           ",16);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
   		tmp_green = (brush_color>>5)/6;
        sprintf(brush,"      %d",tmp_green);
        for(i = 0 ;brush[i] != '\0';i++){

        }
        for(j = i;j<16;j++){
            brush[j] = ' ';
        }
        write(text_dev,brush,16);
        strcommand.pos = 0;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
		
		break;
    case 'e':
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
        write(text_dev,"Eraser          ",16);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
		break;
    case '+':
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
        write(text_dev,"Brush bigger     ",16);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
		sprintf(brush,"      %d",brush_size);
        for(i = 0 ;brush[i] != '\0';i++){

        }
        for(j = i;j<16;j++){
            brush[j] = ' ';
        }
		write(text_dev,brush,16);
        strcommand.pos = 0;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
        break;
    case '-':
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
        write(text_dev,"Brush smaller    ",16);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
		sprintf(brush,"      %d",brush_size);
		for(i = 0 ;brush[i] != '\0';i++){
				
		}
		for(j = i;j<16;j++){
			brush[j] = ' ';
		}
        write(text_dev,brush,16);
        strcommand.pos = 0;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
		break;
    case 'f':
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
        write(text_dev,"FACE DETECT      ",16);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
		break;

    case 'y':
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
        write(text_dev,"GRAY SCALE      ",16);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
		break;	
	
    case 'x':
        ioctl(text_dev,TEXTLCD_CLEAR,&strcommand,32);
        write(text_dev,"SET DIPSW 255   ",16);
        strcommand.pos = 40;
        ioctl(text_dev,TEXTLCD_DD_ADDRESS,&strcommand,32);
                break; 
    default:
		break;
	}
}

void close_textlcd(){
	close(text_dev);
}
