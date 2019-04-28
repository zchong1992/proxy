#include "log.h"
#include "aes.h"

using namespace std;

namespace baseservice
{
#ifdef ZWINDOWS
void usleep(int time) {
    Sleep(time / 1000);
    return;
}
#endif
znlog *znlog::Instance = 0;

#ifdef ZLINUX
LOCKER znlog::static_log_locker = PTHREAD_MUTEX_INITIALIZER;
#endif
#ifdef ZWINDOWS
LOCKER znlog::static_log_locker = 0;
#endif

void znlog::lock_g_locker() {
    PTHREAD_LOCK(&static_log_locker);
}
void znlog::init_g_locker() {
    PTHRAED_INIT(&static_log_locker);
}
void znlog::unlock_g_locker() {
    PTHREAD_UNLOCK(&static_log_locker);
}

znlog *znlog::getInstance() {
    if (Instance == NULL) {
        lock_g_locker();
        if (Instance == NULL) {
            Instance = new znlog();
            //createthread(logsockThread,Instance);
        }
        unlock_g_locker();
    }
    return Instance;
}
void znlog::SYS_LOG2(const int level, const char *fmt, ...) {
    znlog *znlogpointer = znlog::getInstance();
    char str[2048];

    const char *plevel = "[null]";
    switch (level) {
    case ZLOGINFO:
        plevel = "[INFO]";
        break;
    case ZLOGWARNING:
        plevel = "[WARNING]";
        break;
    case ZLOGERROR:
        plevel = "[ZLOGERROR]";
        break;
    }
    strcpy(str, plevel);

    const char *timeStr = GetTimeString();

    strcat(str, timeStr);

    int use_len = strlen(str);
    va_list args;
    va_start(args, fmt);
    vsnprintf(str + use_len, 2048 - use_len - 1, fmt, args);
    va_end(args);
    int str_len = strlen(str);
    //          znlogpointer->write(level,timeStr,time_str_len);
    //          znlogpointer->SockSendLog(timeStr,time_str_len);
    znlogpointer->write(level, str, str_len);
    znlogpointer->SockSendLog(str, str_len);
}
void znlog::TEMPLOG(const char *path, const char *fmt, ...) {
    char str[2048];
    char time[2048];
    const char *timeStr = GetTimeString();
    strcpy(time, timeStr);
    va_list args;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
    strcat(time, str);
    strcat(time, "\n");
    FILE *fp = fopen(path, "a+");
    if (fp) {
        fwrite(time, 1, strlen(time), fp);

        if (fp)
            fclose(fp);
        fp = 0;
    }
    printf("%s", time);
}
void znlog::SYS_LOG3(const int level, const char *fmt, ...) {
    znlog *znlogpointer = znlog::getInstance();
    char str[2048];
    va_list args;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
    znlogpointer->write(level, str, strlen(str));
}
void znlog::Init() {
    //init_g_locker();
    znlog *znlogpointer = znlog::getInstance();
    PTHRAED_INIT(&znlogpointer->m_locker);
    PTHRAED_INIT(&znlogpointer->m_locker_change_file);
}

znlog::znlog() {
    m_is_can_write = 0;
    m_file = 0;
    m_cur_level_write = ZLOGINFO;
    m_cur_level_print = ZLOGINFO;
    m_filename[0] = 0;
    PTHRAED_INIT(&m_locker);
    PTHRAED_INIT(&m_locker_change_file);
}
znlog::~znlog() {
    fflush(this->m_file);
    fclose(this->m_file);
    this->m_file = 0;
}
void znlog::set_level(int write, int print) {
    if (write >= ZLOGERROR && write <= ZLOGINFO)
        this->m_cur_level_write = write;
    if (print >= ZLOGERROR && print <= ZLOGINFO)
        this->m_cur_level_print = print;
    if (write == 0)
        this->m_cur_level_write = 0;
    if (print == 0)
        this->m_cur_level_print = 0;
}

void znlog::SockSendLog(const char *buf, int len) {
    if (m_sock > 0) {
        int ret = send(m_sock, buf, len, 0);
        if (ret < 1) {
            PTHREAD_LOCK(&m_locker);
            closesocket(m_sock);
            m_sock = -1;
            PTHREAD_UNLOCK(&m_locker);
        }
    }
}
void znlog::set_sock_addr(const char *ip, unsigned short port) {
    strcpy(m_ip, "0.0.0.0");
    m_port = port;
}

void znlog::startSocketThread() {
    createthread(logsockThread, this);
}
char *showHex(char *addr, int len) {
    static char buf[1024];
    if (len > 1024) {
    }
    return 0;
}
int checkSocketAvialbe(int sock) {
    char buf1[128];
    char buf2[128];
    char buf3[128];
    char buf4[128];
    char iv[20] = {0};
    char getKey[20];
    char setKey[20];
    memcpy(getKey, "zkzndemoandroids", 16); //
    memcpy(setKey, "zkzndemoandroidg", 16);

    AESModeOfOperation maes;
    maes.set_iv((ZUINT1 *)iv);
    maes.set_key((ZUINT1 *)setKey);
    maes.set_mode(MODE_CBC);
    int outlen = 0;
    memcpy(buf1, "1234567890\0", 11);
    outlen = maes.Encrypt((ZUINT1 *)buf1, 16, (ZUINT1 *)buf2);
    SYS_LOG(ZLOGINFO, "send data %s to  log socket\n", buf1);
    int ret = send(sock, buf2, 16, 0);
    if (ret <= 0) {
        SYS_LOG(ZLOGINFO, "send log socket fail\n");
        return 0;
    }
    int recvlen = 0;
    maes.set_iv((ZUINT1 *)iv);
    maes.set_key((ZUINT1 *)setKey);
    maes.set_mode(MODE_CBC);
    outlen = maes.Decrypt((ZUINT1 *)buf2, 16, (ZUINT1 *)buf4);
    SYS_LOG(ZLOGINFO, "should decode data %s \n", buf4);
    while (recvlen < 16) {
        ret = recv(sock, buf3 + recvlen, 16 - recvlen, 0);
        if (ret <= 0) {
            SYS_LOG(ZLOGINFO, "recv log socket fail\n");
            return 0;
        }
        recvlen += ret;
    }
    maes.set_iv((ZUINT1 *)iv);
    maes.set_key((ZUINT1 *)getKey);
    maes.set_mode(MODE_CBC);
    outlen = maes.Decrypt((ZUINT1 *)buf3, 16, (ZUINT1 *)buf4);
    SYS_LOG(ZLOGINFO, "recv data %s to  log socket\n", buf4);
    if (memcmp(buf1, buf4, 11) == 0)
        return 1;
    return 0;
}
void *znlog::logsockThread(void *Para) {
    set_thread_title("logsockThread");
    SYS_LOG(ZLOGINFO, "sockThread satart\n");
    znlog *m_fd = (znlog *)Para;
    os_socket sockSrv = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrSrv;
    int flag = 1, len = sizeof(int);
    addrSrv.sin_addr.s_addr = 0;
    addrSrv.sin_port = htons(m_fd->m_port);
    addrSrv.sin_family = AF_INET;
    if (setsockopt(sockSrv, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, len) == -1) {
        SYS_LOG(ZLOGINFO, "setsockopt fail");
        exit(0);
    }
    if (bind(sockSrv, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR)) != 0) {
        SYS_LOG(ZLOGERROR, "bind %d fail\n", m_fd->m_port);
        closesocket(sockSrv);
        exit(0);
    }
    listen(sockSrv, 1);
    SOCKADDR_IN addrClient;
    len = sizeof(SOCKADDR_IN);

    while (1) {
        int tmp_sock = 0;
        tmp_sock = accept(sockSrv, (SOCKADDR *)&addrClient, (socklen_t *)&len);
        SYS_LOG(ZLOGINFO, "recv a log request \n");
        if (checkSocketAvialbe(tmp_sock) == 0) {
            SYS_LOG(ZLOGINFO, "check log socket fail \n");
            closesocket(tmp_sock);
            continue;
        }
        SYS_LOG(ZLOGINFO, "check log socket success\n");
        PTHREAD_LOCK(&m_fd->m_locker);
        if (m_fd->m_sock <= 0) {
            m_fd->m_sock = tmp_sock;
        } else {
            SYS_LOG(ZLOGWARNING, "may have two client work togetter m_sock<=0 fail %d\n", m_fd->m_sock);
            closesocket(tmp_sock);
        }
        PTHREAD_UNLOCK(&m_fd->m_locker);
        while (m_fd->m_sock > 0) {
            usleep(100);
        }
    }
}
int znlog::write(const int level, const char *str, const int len) {
    int ret = 0;
    if (str == 0)
        return -1;
    if (level != 0 && this->m_cur_level_write >= level) {
        PTHREAD_LOCK(&this->m_locker);
        if (this->m_file == 0) {
            this->m_file = fopen(this->m_filename, "a+");
            if (this->m_file == 0) {
                ret = -2;
                PTHREAD_UNLOCK(&this->m_locker);
                goto WRITEEND;
            }
        }
        int curlen = ftell(this->m_file);
        const int constfileSize = 50 * (1 << 20);
        //printf("curlen log size %d\n",curlen);
        if (curlen > constfileSize) {
            PTHREAD_LOCK(&m_locker_change_file);
            const int bufferSize = 10 << 20;
            int curlen = ftell(this->m_file);
            if (curlen > constfileSize) {
                char *buf = (char *)malloc(bufferSize + 1);
                if (buf) {
                    FILE *fp = fopen(this->m_filename, "r");
                    if (fp) {
                        fseek(fp, -1 * (bufferSize), SEEK_END);
                        fread(buf, 1, bufferSize, fp);
                        fclose(fp);
                    } else {
                        free(buf);
                        buf = 0;
                    }
                }
                fclose(this->m_file);
                this->m_file = fopen(this->m_filename, "w");
                if (buf) {
                    fwrite(buf, 1, bufferSize, this->m_file);
                    free(buf);
                }
                buf = 0;
            }
            PTHREAD_UNLOCK(&m_locker_change_file);
        }
        fwrite(str, len, 1, this->m_file);
        fflush(this->m_file);
        string mystr = str;
        PTHREAD_UNLOCK(&this->m_locker);
    }
    if (level != 0 && this->m_cur_level_print >= level) {
        printf("%s", str);
        fflush(stdout);
    }
WRITEEND:
    return ret;
}
int znlog::set_log_file(const char *name) {
    if (name == 0)
        return -1;
    int len = strlen(name);
    if (len > 1023) {
        return -2;
    }
    if (len == 0)
        return -3;
    memcpy(this->m_filename, name, len);
    this->m_filename[len] = 0;
    if (this->m_file == 0)
        this->m_file = fopen(this->m_filename, "a+");
    else {
        fclose(this->m_file);
        this->m_file = fopen(this->m_filename, "a+");
    }
    return 0;
}
} // namespace baseservice
