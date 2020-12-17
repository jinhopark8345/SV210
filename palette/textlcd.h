
#ifndef TEXTLCD_H
#define TEXTLCD_H

#define TEXTLCD_BASE            0xbc
#define TEXTLCD_COMMAND_SET     _IOW(TEXTLCD_BASE,0,int)
#define TEXTLCD_FUNCTION_SET    _IOW(TEXTLCD_BASE,1,int)
#define TEXTLCD_DISPLAY_CONTROL _IOW(TEXTLCD_BASE,2,int)
#define TEXTLCD_CURSOR_SHIFT    _IOW(TEXTLCD_BASE,3,int)
#define TEXTLCD_ENTRY_MODE_SET  _IOW(TEXTLCD_BASE,4,int)
#define TEXTLCD_RETURN_HOME     _IOW(TEXTLCD_BASE,5,int)
#define TEXTLCD_CLEAR           _IOW(TEXTLCD_BASE,6,int)
#define TEXTLCD_DD_ADDRESS      _IOW(TEXTLCD_BASE,7,int)
#define TEXTLCD_WRITE_BYTE      _IOW(TEXTLCD_BASE,8,int)

struct strcommand_varible {
        char rows;
        char nfonts;
        char display_enable;
        char cursor_enable;
        char nblink;
        char set_screen;
        char set_rightshit;
        char increase;
        char nshift;
        char pos;
        char command;
        char strlength;
        char buf[16];
};


void pad_string(char* srcStr, char* dstStr);
void init_textlcd();
void textlcd_write(char ch, unsigned short brush_size, unsigned short brush_color);
void close_textlcd();

#endif
