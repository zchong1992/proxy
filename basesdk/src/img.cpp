

#include "img.h"


namespace baseservice
{
     int zkznImg::jpeg2buf(UINT1 *dst,INT4 *buflen,UINT1 * inbuf,INT4 width ,INT4 height,INT4 channels,INT4 quality)
     {
#if 0
          struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;         // target file 
        JSAMPROW row_pointer[1];        // pointer to JSAMPLE row[s] 
        int     row_stride;             // physical row width in image buffer 


          if(quality>100||quality<10)
          {
               SYS_LOG(INFO,"quality error (%d)\n",quality);
               return 0;
          }
          cinfo.err = jpeg_std_error(&jerr);
          jpeg_create_compress(&cinfo);
          unsigned long  outSize=0;
          UINT1 * memtmp=0;
           jpeg_mem_dest(&cinfo,&memtmp,&outSize);
     
           cinfo.image_width = width;       // image width and height, in pixels
           cinfo.image_height = height;
           if(channels==3)
           {
                cinfo.input_components = 3;           // # of color components per pixel //
                cinfo.in_color_space = JCS_RGB;           // colorspace of input image 

           }
           else if(channels==1)
           {
                cinfo.input_components = 1;           // # of color components per pixel 
                cinfo.in_color_space = JCS_GRAYSCALE;           // colorspace of input image 
                
           }
           else
           {
               SYS_LOG(INFO,"channels error (%d)\n",channels);
                return 0;
           }
           if(*buflen <width *height *channels)
           {
               SYS_LOG(INFO,"buflen error(%d )\twidth (%d) \theight(%d)\tchannels(%d) \n",*buflen,width,height,channels);
                return 0;
           }
           
     
           jpeg_set_defaults(&cinfo);
           jpeg_set_quality(&cinfo, quality, TRUE); //* limit to baseline-JPEG values 
     
           jpeg_start_compress(&cinfo, TRUE);
     
           row_stride = width * channels; // JSAMPLEs per row in image_buffer 
     
           while (cinfo.next_scanline < cinfo.image_height) {
                //这里我做过修改，由于jpg文件的图像是倒的，所以改了一下读的顺序
                //这是原代码：row_pointer[0] = & bits[cinfo.next_scanline * row_stride];
                #if 0
                row_pointer[0] = & inbuf[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];
                #else
                row_pointer[0] = & inbuf[cinfo.next_scanline * row_stride];
                #endif
                (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
           }
     
           jpeg_finish_compress(&cinfo);
           if(memtmp==0)
           {
                SYS_LOG(INFO,"malloc  buf  error \n");
                return 0;
           }
           if(outSize==0)
           {
                SYS_LOG(INFO,"jpeg type  error \n");
                if(memtmp!=0)
                {
                     free(memtmp);
                    memtmp=0;
               }
               return 0;
           }
           memcpy(dst,memtmp,outSize);
           *buflen=outSize;
           jpeg_destroy_compress(&cinfo);
           if(memtmp!=0)
           {
                free(memtmp);
                memtmp=0;
           }
           
           return outSize;
 #else
         return -1;
 #endif
     }

     int  zkznImg::createbmpheader(unsigned char *buf,int *length,const int width,const int height,const int channel)
    {
        if(channel==3||channel==4)
        {
             if(*length<(int)sizeof(BITMAP_FILE))
                  return -1;
             BITMAP_FILE *bmp=(BITMAP_FILE*)(buf);
             memset(&bmp->header,0,sizeof(bmp->header));
             memset(&bmp->infoheader,0,sizeof(bmp->infoheader));
             bmp->header.bfType=*(unsigned int*)"BM";
             bmp->header.bfSize=sizeof(BITMAP_FILE)+width*height*channel;
             bmp->header.bfOffBits=sizeof(BITMAP_FILE);
             bmp->header.bfReserved1=0;
             bmp->header.bfReserved2=0;
             bmp->infoheader.biSize=sizeof(bmp->infoheader);
             bmp->infoheader.biWidth=width;
             bmp->infoheader.biHeight=height;
             bmp->infoheader.biPlanes=1;
             bmp->infoheader.biBitCount=channel*8;
        
             bmp->infoheader.biClrUsed=0;
             bmp->infoheader.biCompression=BI_RGB;
             bmp->infoheader.biSizeImage=width*height*channel/8;
             bmp->infoheader.biXPelsPerMeter=0;
             bmp->infoheader.biYPelsPerMeter=0;
             bmp->infoheader.biClrImportant=0;
             *length=bmp->header.bfOffBits;
             return 0;
        }
        else if(channel==1)
        {
             if(*length<(int)sizeof(BITMAP_FILE)+1024)
                  return -1;
             BITMAP_FILE *bmp=(BITMAP_FILE*)(buf);
             memset(&bmp->header,0,sizeof(bmp->header));
             memset(&bmp->infoheader,0,sizeof(bmp->infoheader));
             bmp->header.bfType=*(unsigned int*)"BM";
             bmp->header.bfOffBits=sizeof(BITMAP_FILE)+1024;
             bmp->header.bfSize=bmp->header.bfOffBits+width*height*channel;
             bmp->header.bfReserved1=0;
             bmp->header.bfReserved2=0;
             bmp->infoheader.biSize=sizeof(bmp->infoheader);
             bmp->infoheader.biWidth=width;
             bmp->infoheader.biHeight=height;
             bmp->infoheader.biPlanes=1;
             bmp->infoheader.biBitCount=channel*8;
        
             bmp->infoheader.biClrUsed=0;
             bmp->infoheader.biCompression=BI_RGB;
             bmp->infoheader.biSizeImage=width*height*channel/8;
             bmp->infoheader.biXPelsPerMeter=0;
             bmp->infoheader.biYPelsPerMeter=0;
             bmp->infoheader.biClrImportant=0;
            int index=0;
            
            for(index=0;index<1024;index+=4)
            {
                buf[sizeof(BITMAP_FILE)+index+0]=(unsigned char)(index>>2);
                buf[sizeof(BITMAP_FILE)+index+1]=(unsigned char)(index>>2);
                buf[sizeof(BITMAP_FILE)+index+2]=(unsigned char)(index>>2);
                buf[sizeof(BITMAP_FILE)+index+3]=(unsigned char)0;
            }
             *length=bmp->header.bfOffBits;
             return 0;
        }
        return -1;
    }
}











