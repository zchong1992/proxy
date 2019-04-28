#include "config.h"
#include "log.h"
namespace baseservice
{

#if 0
#define CONFIGLOG printf
#else
#define CONFIGLOG(...) SYS_LOG(ZLOGINFO,##__VA_ARGS__)
#endif
char *strdb(char *str) {
    if (str == 0)
        return 0;
    int len = strlen(str);
    char *a, *b;
    int hask = 0;
    a = b = str;
    for (; b < str + len; b++) {
        if (*b == ' '||*b=='.'||*b==','||*b=='.') {
            continue;
        } else if (*b == '#' || *b == '\r' || *b == '\n') {
            *b = 0;
            return str;
        } else if (*b == '[' && hask == 0) {
            *a = *b;
            a++;
            hask = 1;
        } else if (*b == ']' && hask == 1) {
            *a = *b;
            a++;
            *a = 0;
            return str;
        } else if ((*b >= '0' && *b <= '9') || (*b >= 'A' && *b <= 'Z') || (*b >= 'a' && *b <= 'z') || (*b == ' ' || *b == '_') || *b == '[' || *b == ']' || *b == '+' || *b == '-') {
            *a = *b;
            a++;
        } else {
            return 0;
        }
    }
    *a = 0;
    return str;
}

int charandnum(char *str) {
    if (str == 0)
        return 0;
    int hask = 0;
    int len = strlen(str);
    char *a = str;
    do
    {
        if ((*a >= '0' && *a <= '9') || (*a >= 'A' && *a <= 'Z') || (*a >= 'a' && *a <= 'z'))
            a++;
        else if (*a == ' ' || *a == '_' || *a == '+' || *a == '-')
            a++;
        else if (*a == '[' || *a == ']') {
            a++;
        } else
            return 0;
    } while (a < str + len);
    return 1;
}
znconfig::znconfig() {
    m_zkv = 0;
}

znconfig::~znconfig() {
}
int znconfig::insertData(const char *key, const char *value) {
    if (key == 0 || value == 0) {
        return 0;
    }
    CONFIGLOG("add %s\n", key);
    zkv *pn, *pl;
    if (m_zkv == 0) {
        m_zkv = new zkv();
        m_zkv->key = key;
        m_zkv->value = value;
        m_zkv->next = 0;
        return 1;
    } else {
        pn = pl = m_zkv;
        while (pn->next != 0) {
            //            if(strcmp(pl->key->c_str(),key)>0)
            //            {
            //            }
            //            else if(strcmp(pl->key->c_str(),key)<=0)
            //            {
            //            }
            //            pl=pn;

            pn = pn->next;
        }
        pn->next = new zkv();
        pn = pn->next;
        pn->key = key;
        pn->value = value;
        pn->next = 0;
        return 1;
    }
}
int znconfig::save(const char *path) {
    zkv *pn;
    pn = m_zkv;
    if (pn == 0) {
        return -1;
    }
    if (path == 0)
        return -1;
    FILE *fp = fopen(path, "w");
    if (fp == 0)
        return -1;
    pn = m_zkv;
    do
    {
        fprintf(fp, "[%s]\n%s\n", pn->key.c_str(), pn->value.c_str());
        pn = pn->next;
    } while (pn != 0);
    fclose(fp);
    return 0;
}
int znconfig::add(const char *key, const char *vlaue) {
    return 1;
}
int znconfig::del(const char *key) {
    return 1;
}
const char *znconfig::Getvalue(const char *key) {
    zkv *pn;
    pn = m_zkv;
    if (pn != 0) {
        do
        {
            if (strcmp(pn->key.c_str(), key) == 0) {
                return pn->value.c_str();
            }
            pn = pn->next;
        } while (pn != 0);
    }
    return 0;
}
int znconfig::print() {
    zkv *pn;
    pn = m_zkv;
    SYS_LOG(3, "---------------------------------------------\n");
    if (pn != 0) {
        do
        {
            SYS_LOG(3, "%s\t\t%s\n", pn->key.c_str(), pn->value.c_str());
            pn = pn->next;
        } while (pn != 0);
    }
    SYS_LOG(3, "---------------------------------------------\n");
    return 0;
}
int znconfig::loadfile(const char *filename) {
    char buf[10240];
    int line, complier;
    char *ret;
    char key[1024];
    char value[1024];
    FILE *fp = 0;
    if (filename == 0)
        goto ERRORDEAL;
    fp = fopen(filename, "r");
    if (fp == 0)
        goto ERRORDEAL;
    complier = 0;
    line = 0;
    key[0] = 0;
    value[0] = 0;
    do
    {
        memset(buf, 0, sizeof(buf));
        ret = fgets(buf, sizeof(buf), fp);
        line++;
        if (ret == 0)
            break;
        CONFIGLOG("deal before:%s\n", buf);
        if (strdb(buf) == 0) {
            CONFIGLOG("strdb error ");
            goto ERRORDEAL;
        }
        int len = strlen(buf);
        CONFIGLOG("deal %d:%s\n", len, buf);
        if (len == 0)
            continue;
        if (complier == 0 && buf[0] == '[') {
            complier = 1;
            if (buf[len - 1] == ']') {
                CONFIGLOG("key:%s\n", buf);
                memcpy(key, buf + 1, len - 2);
                key[len - 2] = 0;
            } else {
                CONFIGLOG("no found ] %d %d %s", key[len - 1], len, buf);
                goto ERRORDEAL;
            }
        } else if (complier == 1 && buf[0] != '[') {
            if (charandnum(buf) == 1) {
                CONFIGLOG("vaule:%s\n\n\n", buf);
                memcpy(value, buf, len);
                value[len] = 0;
                insertData(key, value);
                complier = 0;
            } else {
                CONFIGLOG("some charater error! %s %d", buf, len);
                goto ERRORDEAL;
            }
        } else {
            CONFIGLOG("something error!02");
            goto ERRORDEAL;
        }

    } while (!feof(fp));

    if (fp)
        fclose(fp);
    fp = 0;
    return 0;
ERRORDEAL:
    if (fp) {
        fclose(fp);
        fp = 0;
        CONFIGLOG("\tat line:%d\n", line);
    }
    return -1;
}

} // namespace baseservice
