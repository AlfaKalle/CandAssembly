#include <time.h>
#include <stdio.h>
#include <dos.h>
#include "vga.h"
#include "3d.h"

#define MAX_C 30

#define POINT_C 180 
#define POINT_CHALF 90

unsigned char *buffer;
unsigned int *ibuffer;

/* Piirtää annettuun taulukkoon (kuvaan) horisontaalin(vaakasuoran) suoran
linjan vasemmasta laidasta oikeaan laitaan.*/
void drawHline(unsigned char *buf, int y, int col)
{
    int x = 0;
    for(x = 0; x < 320; ++x)
    {
        buf[y * 320 + x] = col;
    }
}

/* Piirtää annettuun taulukkoon (kuvaan) horisontaalin(vaakasuoran) suoran
linjan annetusta vasemmalla olevasta alkukohdasta annettuun loppukohtaan.*/
void drawHlinese(unsigned char *buf, int y, int xs, int es, int col)
{
    int x = 0;
    for(x = xs; x < es; ++x)
    {
        buf[y * 320 + x] = col;
    }
}

/* Piirtää annettuun taulukkoon (kuvaan) vertikaalin(pystysuoran) suoran
linjan ylä laidasta ala lait+aan.*/
void drawVline(unsigned char *buf, int x, int col)
{
    int y = 0;
    for(y = 0;y<200; ++y)
    {
        buf[y * 320 + x] = col;
    }
}

/* Piirtää annettuun taulukkoon (kuvaan) suorakulmion
annetulla värillä.*/
void drawRectangle(unsigned char *buf, int sx, int sy, int ex, int ey, int col)
{
    int x, y;
    for(y = sy;y<=ey;y++)
    {
        for(x=sx;x<=ex;x++)
        {
            buf[y * 320 + x] = col;
        }
    }
}

/* Piirtää janan, mahdollisesti vinon.
Funktio ei ole valmis, ei tietyissä tilanteissa piirrä yhtenäistä suoraa ja tietyissä tilanteissa
epäonnistuu täysin, toimii kuitenkin tämän ohjelman puitteissa tarpeeksi hyvin.*/
void drawLine(unsigned char *buf, int sx, int sy, int ex, int ey, int col)
{
    int deltaX = ex - sx;
    int deltaY = ey - sy;
    int xadd = (deltaX * 100) / deltaY;
    int x, y;

    for(y = 0;y + sy <= ey;y++)
    {
        buf[(y + sy) * 320 + (sx + ((y * xadd) / 100))] = col;
    }
}

/* tx translation on x, ty translation on y, tz translation on z */
/* siirrot akselien suuntiin, tx, ty, tz. */
void transAndDrawLine(struct Point3d *points, int i1, int i2, int tx, int ty, int tz, int col)
{
    int xx = (int)((points[i1].x + tx) / (points[i1].z + tz)) + 160; /*160 screen center x*/
    int yy = (int)((points[i1].y + ty) / (points[i1].z + tz)) + 100; /*100 screen center y*/

    int xx2 = (int)((points[i2].x + tx) / (points[i2].z + tz)) + 160;
    int yy2 = (int)((points[i2].y + ty) / (points[i2].z + tz)) + 100;

    drawLine(buffer, xx, yy, xx2, yy2, col);
}

void transAndDrawRectangle(struct Point3d *points, int i1, int i2, int tx, int ty, int tz, int col)
{
    int xx = (int)((points[i1].x + tx) / (points[i1].z + tz)) + 160;
    int yy = (int)((points[i1].y + ty) / (points[i1].z + tz)) + 100;

    int xx2 = (int)((points[i2].x + tx) / (points[i2].z + tz)) + 160;
    int yy2 = (int)((points[i2].y + ty) / (points[i2].z + tz)) + 100;

    drawRectangle(buffer, xx, yy, xx2, yy2, col);
}

int main() {

    union REGS regs;
    int xx, yy, xx2, yy2;
    int i, j, t;

    int screen_center_x = 160;
    int screen_center_y = 100;

    struct Point3d points[POINT_C];
    struct Point3d *pointbuffer = points;
    struct Point3d boxpoints[8];

    buffer = malloc(200*320);
    ibuffer = (unsigned int *)buffer;

    /* ns. tien reunapisteet */
    for(i =0;i<POINT_CHALF;i++)
    {
        pointbuffer[i].x = -10000;
        pointbuffer[i].y = 10000;
        pointbuffer[i].z = 110 + (i * 20);

        pointbuffer[i + POINT_CHALF].x = 10000;
        pointbuffer[i + POINT_CHALF].y = 10000;
        pointbuffer[i + POINT_CHALF].z = 110 + (i * 25);
    }

    /*Laatikon nurkkapisteet järjestyksessä: vasen etu ylä kulma -> vasen taka alakulma*/
    boxpoints[0].x = -1000;
    boxpoints[0].y = -1000;
    boxpoints[0].z =   -50;

    /*oikea etu yläkulma*/
    boxpoints[1].x =  1000;
    boxpoints[1].y = -1000;
    boxpoints[1].z =   -50;

    /*oikea etu alakulma*/
    boxpoints[2].x =  1000;
    boxpoints[2].y =  1000;
    boxpoints[2].z =   -50;
    
    /*vasen etu alakulma*/
    boxpoints[3].x = -1000;
    boxpoints[3].y =  1000;
    boxpoints[3].z =   -50;

    /*vasen taka yläkulma*/
    boxpoints[4].x = -1000;
    boxpoints[4].y = -1000;
    boxpoints[4].z =    50;

    /*oikea taka yläkulma*/
    boxpoints[5].x =  1000;
    boxpoints[5].y = -1000;
    boxpoints[5].z =    50;

    /*oikea taka alakulma*/
    boxpoints[6].x =  1000;
    boxpoints[6].y =  1000;
    boxpoints[6].z =    50;

    /*vasen taka alakulma*/
    boxpoints[7].x = -1000;
    boxpoints[7].y =  1000;
    boxpoints[7].z =    50;

    regs.h.ah = 0;
    regs.h.al = 0x13;
    int86(0x10, &regs, &regs);

    for(j=-75;j<75;j++)
    { 
        fillbuffer(ibuffer, 0);

        for(i=0;i<8;i++)
        {

            xx = (int)((boxpoints[i].x - j * 100) / 100) + screen_center_x;
            yy = (int)(boxpoints[i].y / 100) + screen_center_y;

            xx = (int)((boxpoints[i].x - j * 100) / (boxpoints[i].z + 250)) + screen_center_x; 
            yy = (int)(boxpoints[i].y / (boxpoints[i].z + 250)) + screen_center_y; 

            if(xx >= 0 && xx < 320)
            {
                buffer[yy * 320 + xx] = 10; 
            }
        }

        for(i=0;i<POINT_CHALF;i++)
        {
            xx = (int)((pointbuffer[i].x - j * 100) / pointbuffer[i].z) + screen_center_x;
            yy = (int)(pointbuffer[i].y / pointbuffer[i].z) + screen_center_y;

        /*    buffer[yy * 320 + xx] = 10; */

            t = i + POINT_CHALF;
            xx2 = (int)((pointbuffer[t].x - j * 100) / pointbuffer[t].z) + screen_center_x;
            yy2 = (int)(pointbuffer[t].y / pointbuffer[t].z) + screen_center_y; 

        /*    buffer[yy2 * 320 + xx2] = 10; */

            drawHlinese(buffer, yy, xx, xx2, i + j);
        } 

        /*Piirtää vasemman laatikon */
        transAndDrawRectangle(boxpoints, 4, 6, j * -100 - 5000, -4000, 250, 10);

        transAndDrawLine(boxpoints, 0, 4, j * -100 - 5000, -4000, 250, j); 
        transAndDrawLine(boxpoints, 1, 5, j * -100 - 5000, -4000, 250, j); 
        transAndDrawLine(boxpoints, 2, 6, j * -100 - 5000, -4000, 250, j); 
        transAndDrawLine(boxpoints, 3, 7, j * -100 - 5000, -4000, 250, j); 

        transAndDrawRectangle(boxpoints, 0, 2, j * -100 - 5000, -4000, 250, 12); 

        /*Piirtää oikean laatikon */
        transAndDrawRectangle(boxpoints, 4, 6, j * -100 + 5000, -4000, 250, 10);

        transAndDrawLine(boxpoints, 0, 4, j * -100 + 5000, -4000, 250, j);
        transAndDrawLine(boxpoints, 1, 5, j * -100 + 5000, -4000, 250, j); 
        transAndDrawLine(boxpoints, 2, 6, j * -100 + 5000, -4000, 250, j); 
        transAndDrawLine(boxpoints, 3, 7, j * -100 + 5000, -4000, 250, j); 

        transAndDrawRectangle(boxpoints, 0, 2, j * -100 + 5000, -4000, 250, 12); 
        
        buffertovga(ibuffer); 
    }

    regs.h.ah = 0;
    regs.h.al = 0x12;
    int86(0x10, &regs, &regs);

    exit(0);

    return 0;
}