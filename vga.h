#ifndef VGA_H
#define VGA_H

/* Asettaa yksittäisen pikselin annetussa paikassa annettuun väriin */
static void setpixelvga(int x, int y, int color)
{
#asm
  push  bp
  mov   bp,sp
  push  ds
  mov   bx,[bp+6]  ! y koordinaatti
  imul  bx,#320    ! kerrotaan rivin leveydellä
  add   bx,[bp+4]  ! rekisteriin bx tulee osoite y * 320 + x
  mov   ax,#$A000  ! VGA segmentin osoite
  mov   ds,ax
  mov   ax,[bp+8]  ! väri
  mov   [bx],al
  pop   ds
  pop   bp
#endasm
}

/* Asettaa 2 vierekkäistä pikseliä annettuun väriin annetussa paikassa */
static void setpos16bitvga(int pos, unsigned int color)
{
#asm
  push  bp
  mov   bp,sp
  push  ds
  mov   bx,[bp+4]  ! sijainti
  imul  bx,#2
  mov   ax,#$A000  ! VGA segmentin osoite
  mov   ds,ax
  mov   ax,[bp+6]  ! asetetaan paikkaan väri
  mov   [bx],ax
  pop   ds
  pop   bp
#endasm
}

/* Täyttää annetun taulukon annetulla värillä (luvulla) */
static void fillbuffer(unsigned int *buffer, unsigned int color)
{
#asm
  push  bp
  mov   bp,sp
  mov   bx,[bp+4]
  mov   cx,bx
  add   cx,#64000
  mov   ax,[bp+6]
next:
  mov   [bx],ax
  add   bx,#2
  cmp   bx,cx
  jne   next
  pop   bp
#endasm
}

/* Siirtää näytön kokokoisen taulukon näyttömuistiin eli täyttää koko näytön taulukossa olevalla kuvalla */
static void buffertovga(unsigned int *buffer)
{
#asm
  push  bp
  mov   bp,sp
  push  ds
  mov   cx,ds
  mov   ax,#0
begin:
  mov   bx,[bp+4]
  add   bx,ax
  mov   dx,[bx]
  push  ax
  mov   ax,#$A000
  mov   ds, ax
  pop   ax
  mov   bx,ax
  mov   [bx],dx
  add   ax,#2
  mov   ds,cx
  cmp   ax,#64000
  jne   begin
  pop   ds
  pop   bp
#endasm
}

/* Periaatteessa tehokkaampi versio edellisestä metodista */
static void buffertovga8bytes(unsigned int *buffer)
{
#asm
  push  bp
  mov   bp,sp
  push  ds
  mov   cx,ds
  mov   ax,#0
beginfour:
  mov   bx,[bp+4]
  add   bx,ax
  mov   dx,[bx]
  push  ax
  mov   ax,#$A000
  mov   ds, ax
  pop   ax
  mov   bx,ax
  mov   [bx],dx
  add   ax,#2
  mov   ds,cx

  mov   bx,[bp+4]
  add   bx,ax
  mov   dx,[bx]
  push  ax
  mov   ax,#$A000
  mov   ds, ax
  pop   ax
  mov   bx,ax
  mov   [bx],dx
  add   ax,#2
  mov   ds,cx

  mov   bx,[bp+4]
  add   bx,ax
  mov   dx,[bx]
  push  ax
  mov   ax,#$A000
  mov   ds, ax
  pop   ax
  mov   bx,ax
  mov   [bx],dx
  add   ax,#2
  mov   ds,cx

  mov   bx,[bp+4]
  add   bx,ax
  mov   dx,[bx]
  push  ax
  mov   ax,#$A000
  mov   ds, ax
  pop   ax
  mov   bx,ax
  mov   [bx],dx
  add   ax,#2
  mov   ds,cx

  cmp   ax,#64000
  jne   beginfour
  pop   ds
  pop   bp
#endasm
}

#endif
