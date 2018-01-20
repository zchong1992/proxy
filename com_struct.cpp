
#include "com_struct.h"

using namespace baseservice;
using namespace std;

socketPair::socketPair()
{
    memset(this,0,sizeof(socketPair));
}
socketPair::socketPair(const socketPair &_A)
{
    memcpy(this,&_A,sizeof(socketPair));
}
socketPair & socketPair::operator=(const socketPair &_A)
{
    memcpy(this,&_A,sizeof(socketPair));
    return *this;
}

int max(int a,int b)
{
    return a>b?a:b;
}

int check_is_ip(const char*str)
{
    int len=strlen(str);
    int index=0;
    for(index=0;index<len;index++)
    {
        if(str[index]=='.')
            continue;
        if(str[index]>='0'&&str[index]<='9')
            continue;
        return 0;
    }
    return 1;
}


void closeFd(VSP &A,VSPI it)
{
    close((*it).clientFd);
    close((*it).remoteFd);
    SYS_LOG(INFO,"close fd pair,(%d,%d,%d,%d)\n"
        ,(*it).clientData[0]
        ,(*it).clientData[1]
        ,(*it).remoteData[0]
        ,(*it).remoteData[1]);
    A.erase(it);
}
int hostname_to_ip(const char * hostname , char* ip)
{
    if(check_is_ip(hostname))
    {
        strcpy(ip,hostname);
        return 1;
    }
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
          
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
  
    addr_list = (struct in_addr **) he->h_addr_list;
      
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip ,inet_ntoa(*addr_list[i]) );
        return 1;
    }
      
    return 0;
}




