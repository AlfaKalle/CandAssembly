#include <TIME.H>
#include <stdio.h>
#include <dos.h>
#include "vga.h"

#define MAX_C 50

void drawHline(unsigned char *buf, int y, int col)
{
    int x = 0;
    for(x = 0; x < 320; ++x)
    {
        buf[y * 320 + x] = col;
    }
}

void drawVline(unsigned char *buf, int x, int col)
{
    int y = 0;
    for(y = 0;y<200; ++y)
    {
        buf[y * 320 + x] = col;
    }
}

int main() {

    int done = 0;
    int x, y, c;
    unsigned int cc = 0;
    unsigned int i;
    unsigned char *buffer = malloc(200*320);
    unsigned int *ibuffer = (unsigned int *)buffer;
    char character;
    int xx, yy;
    clock_t start, stop;
    long t1, t2, t3, t4;

    union REGS regs;
    regs.h.ah = 0;
    regs.h.al = 0x13;
    int86(0x10, &regs, &regs);

    c = 0;

    start = time();

    while(done == 0)
    {  
        for(y = 0; y < 200; ++y)
        {
            
            for(x = 0; x < 320; ++x)
            {
                setpixelvga(x, y, c);           
            }

        }

        c++;
        if(c > MAX_C)
        {
            c = 0;
            done = 1;
        }      
    }

    stop = time();
    t1 = stop - start;

    done = 0;

    start = time();

    while(done == 0)
    {      
        fillbuffer(ibuffer, cc);

        for(i=0;i < 32000; ++i)
        {
            setpos16bitvga(i, ibuffer[i]);            
        }

        c++;
        cc = c + (c << 8);
        if(c > MAX_C)
        {
            c = 0;
            done = 1;
        }      
    }

    stop = time();
    t2 = stop - start;

    done = 0;

    start = time();

    while(done == 0)
    {
        fillbuffer(ibuffer, cc);
       
        buffertovga(ibuffer);

        c++;
        cc = c + (c << 8);
        if(c > MAX_C)
        {
            c = 0;
            done = 1;
        }      
    }

    stop = time();
    t3 = stop - start;

    done = 0;

    start = time();

    while(done == 0)
    {
        fillbuffer(ibuffer, cc);
       
        buffertovga8bytes(ibuffer);

        c++;
        cc = c + (c << 8);
        if(c > MAX_C)
        {
            c = 0;
            done = 1;
        }      
    }

    stop = time();
    t4 = stop - start;

    fillbuffer(ibuffer, 0);

    drawHline(buffer, 99, 1);
    drawHline(buffer, 100, 1);

    drawVline(buffer, 159, 1);
    drawVline(buffer, 160, 1);

    for (i=0;i<320;++i)
    {
        xx = (i - 159) / 2;
        yy = -xx * xx + 100;

        if(yy < 200 && yy >= 0)
        {
            buffer[yy * 320 + i] = 2;
        }
    }

    for (i=0;i<640;++i)
    {
        xx = (int)(i - 320) / 4;
        yy = (int)(xx * xx * xx / 64) + 100;

        if(yy < 200 && yy >= 0)
        {
            buffer[yy * 320 + (i / 2)] = 3;
        }
    }
         
    buffertovga8bytes(ibuffer);

    printf("First run took time %d . \n", t1);
    printf("Second run took time %d . \n", t2);
    printf("Third run took time %d . \n", t3);
    printf("Forth run took time %d . \n", t4);
 
    return 0;
}