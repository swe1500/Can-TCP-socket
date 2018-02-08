#ifndef CANT_H
#define CANT_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include <arpa/inet.h>
#include <linux/can.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sstream>
#include <fstream>

class Can{
private:
    /***** Can ******/
    int can, canbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    struct can_filter rfilter[3];
    /***** TCP *****/
    int    sockfd, n;
    char   sendline[40];
    struct sockaddr_in   servaddr;

    uint16_t NoOfTarN, NoOfTarF, a, b, c, d, e, f, NoOfTar, Tar_Type;
    float  Vrel, Dist, Tar_Ang;

    std::ofstream RadarRecord;

    void CanConfigure();
    void TcpConfigure(int argc, char** argv);

    template <typename T>
    std::string Num2String(T num);
    void CopyString(char* sendline, std::string str);

public:
    void Update();
    void SendMessage();
    void PrintInfo();
    Can(int argc, char** argv);
    ~Can();
};

#endif // CANT_H
