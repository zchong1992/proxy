#include "com_struct.h"
#include <thread>
#include "libbaseservice.h"
using namespace std;
using namespace baseservice;

int org_main(int argc, char *argv[]);
void test_offline()

{
    char *argv[] = {
        (char *)"offline",
        (char *)"offline",
        (char *)"127.0.0.1",
        (char *)"80",
        (char *)"127.0.0.1",
        (char *)"10011"};
    org_main(6, argv);
}
void test_online()

{
    char *argv[] = {
        (char *)"offline",
        (char *)"online",
        (char *)"10010",
        (char *)"10011"};
    org_main(4, argv);
}
void test_client()
{
    const char *http_header = (char *)"GET / HTTP/1.1\r\n\r\n";
    int http_len = strlen(http_header);
    const int BufSize = 10 * 2 << 20;
    char *UseBuf = (char *)malloc(BufSize);
    if (UseBuf == 0)
    {
        SYS_LOG(0, "malloc UseBuf fail\n");
        return;
    }
    while (1)
    {
        tcpClient tc;

        int ret = tc.init();
        if (ret != 0)
        {
            SYS_LOG(0, "init fail %d\n", ret);
            goto END;
        }
        ret = tc.connect("127.0.0.1", "10010");
        if (ret != 0)
        {
            SYS_LOG(0, "connect fail %d\n", ret);
            goto END;
        }
        ret = tc.send(http_header, http_len, 0);
        if (ret <= 0)
        {
            SYS_LOG(0, "send fail %d\n", ret);
            goto END;
        }
        ret = tc.recv(UseBuf, BufSize, 0);
        if (ret < 0)
        {
            SYS_LOG(0, "recv fail %d\n", ret);
            goto END;
        }
        UseBuf[ret] = 0;
        printf("recv:[%s]\n\n", UseBuf);
    END:
        usleep(10);
    }
    if (UseBuf)
    {
        free(UseBuf);
        UseBuf = 0;
    }
}

int main()
{
    thread online(test_online);
    online.detach();
    thread offline(test_offline);
    offline.detach();
    thread client(test_client);
    client.detach();

    while (1)
    {
        sleep(10);
    }
    return 0;
}