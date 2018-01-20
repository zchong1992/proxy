
#include "dir.h"


int ZCDIR::checkDirExist(const char *Path)
{
     
     return 0;
}

int zclibjustcreateDir(const char* Path)
{

     struct stat file_stat;
     int ret = 0;
     ret = stat(Path, &file_stat);
     if (ret<0)
     {
          if (errno == ENOENT)
          {
               ret = mkdir(Path, 0775);
               if (ret < 0)
               {
                    return 0;
               }

          }
          else
          {
               return 0;
          }
     }
     return 1;
}
int zclibcreateFatherDir(const char *Path)
{
     char buf[10240] = { 0 };
     int len = strlen(Path);
     memcpy(buf, Path, len + 1);
     int index = 0;
     int dircount = 0;
     for (index = len - 1; index > 0; index--)
     {
          char tdot = buf[index];
          if (buf[index] == '/' || buf[index] == '\\')
          {
               break;
          }
     }
     buf[index] = 0;
     int ret = zclibjustcreateDir(Path);
     if (ret == 0)
     {
          ret = zclibcreateFatherDir(buf);
          if (ret == 0)
               return ret;
          int ret = zclibjustcreateDir(Path);
          return ret;
     }
}

int ZCDIR::createDir(const char *Path)
{
    zclibcreateFatherDir(Path);
     return 0;
}
int ZCDIR::deleteDir(const char *Path)
{
     return 0;
}




