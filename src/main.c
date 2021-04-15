#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "FixupZIP.h"
#include "ByteIO.h"

unsigned int Content_len;
unsigned char Content_type[4];
unsigned char *Content_dat;

int main(int argc,char* argv[]){
  FILE *fp;
  set_sysendian();
  if(argc<2){
    printf("usage\n%s input.file [output.bmp]",argv[0]);
    return 0;
  }
  if(argc>2){
    if ((fp = fopen(argv[2], "wb")) == NULL) {
      printf("fopen error:%s",argv[2]);
      return -1;
    }
  }else if ((fp = fopen("output.bmp", "wb")) == NULL) {
    printf("fopen error:%s",argv[2]);
    return -1;
  }

  Content_dat = readFile(argv[1],&Content_len);
  if(strstr(argv[1],".zip")!=NULL){
    printf("This is ZIP file,Fixing up...");
    FixupZIP(Content_dat,Content_len,0x36);
  }

  int Width=0,Height;
  while(1){
    Height=Width/1.4;
    if(3*Width*Height>Content_len)break;
    Width+=4;
  }
  Height=0;
  while(1){
    if(3*Width*Height>Content_len)break;
    Height++;
  }
  uint32_t img_len = Width*Height*3;

  fwrite("BM",2,1,fp);//FileType
  fwriteInt(fp,0x36+img_len,4,'L');//FileSize
  fwrite("\00\00\00\00",2,2,fp);//Reserved*2
  fwriteInt(fp,0x36,4,'L');//Offset
  fwriteInt(fp,40,4,'L');//HeaderSize
  fwriteInt(fp,Width,4,'L');//Width
  fwriteInt(fp,Height,4,'L');//Height
  fwriteInt(fp,1,2,'L');//Planes
  fwriteInt(fp,24,2,'L');//BitSize
  fwriteInt(fp,0x0,4,'L');//Compression
  fwriteInt(fp,0x0,4,'L');//ImageSize
  fwriteInt(fp,3780,4,'L');//XPixPerMeter
  fwriteInt(fp,3780,4,'L');//YPixPerMeter
  fwriteInt(fp,0x0,4,'L');//PLCount
  fwriteInt(fp,0x0,4,'L');//PLIndex_Imp

  fwrite(Content_dat,Content_len,1,fp);
  for(int i = 0;i<img_len-Content_len;i++){
    fwrite("\00",1,1,fp);
  }

  fclose(fp);
  return 0;
}