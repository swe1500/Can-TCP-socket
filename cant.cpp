#include "cant.h"

void Can::TcpConfigure(int argc, char** argv){
    if( argc != 2){
    printf("usage: ./client <ipaddress>\n");
    exit(0);
    }

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
    exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
    printf("inet_pton error for %s\n",argv[1]);
    exit(0);
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
    printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
    exit(0);
    }

}

template <typename T>
std::string Can::Num2String(T num){
    std::ostringstream buff;
    buff<<num;
    return buff.str();
}

void Can::CopyString(char* sendline, std::string str){
    unsigned int i =0;
    for(; i < str.size(); i++)
    sendline[i] = str[i];
        sendline[i]='\n';
}

void Can::Update(){
    canbytes = read(can, &frame, sizeof(frame)); //receive can bus message
    if(canbytes > 0)
        {
          switch (frame.can_id) {
            case 0x600:
              NoOfTarN = frame.data[0], NoOfTarF = frame.data[1];
              break;
            case 0x701:
              NoOfTar = frame.data[0];
              if (((0 < NoOfTar) && (NoOfTar<= NoOfTarN)) || ((33<= NoOfTar) && (NoOfTar<(33+NoOfTarF))) )
              {  a = frame.data[4], b = frame.data[5], c = frame.data[6], d = ((a<<4) | (b & 0xF0)>>4), e = (b & 0x07)<<8 | (c & 0xFF) ;
                 Vrel = (float)d*0.03-25, Dist = (float)e*0.1;
               }
              break;
            case 0x702:
              if ( ((0 < NoOfTar) && (NoOfTar<= NoOfTarN)) || ((33<= NoOfTar) && (NoOfTar<(33+NoOfTarF))) )
              { Tar_Type = frame.data[5]>>6, f = (frame.data[5] & 0x0F)<<8 | (frame.data[6] & 0xFF);
                Tar_Ang = (float)f*0.1-30;
              }
              break;
          }
        }
}

void Can::SendMessage()
{
    std::string str = Num2String(NoOfTarN);
    CopyString(sendline, str);
    send(sockfd,sendline,str.size()+1,0);
}

void Can::PrintInfo(){
    RadarRecord <<"ID=0x"<<std::hex<<frame.can_id<<" NoOfTargetsNear:"<<std::dec<<NoOfTarN<<" NoOfTargetsFar:"<<NoOfTarF<<std::endl;
    printf("ID=0x%X NoOfTargetsNear:%d NoOfTargetsFar:%d \n", frame.can_id, NoOfTarN,NoOfTarF);
    RadarRecord <<"ID=0x"<<std::hex<<frame.can_id<<" NoOfTarget:"<<std::dec<<NoOfTar<<" Velocity:"<<Vrel<<" Distance:"<<Dist<<std::endl;
    printf("ID=0x%X NoOfTarget:%d Velocity:%.01fm/s Distance:%.1fm \n", frame.can_id, NoOfTar,Vrel, Dist);
    RadarRecord <<"ID=0x"<<std::hex<<frame.can_id<<" Tar_Type:"<<std::dec<<Tar_Type<<" Tar_Ang:"<<Tar_Ang<<std::endl;
    printf("ID=0x%X Tar_Type:%d Tar_Ang:%.01fdgree \n", frame.can_id, Tar_Type,Tar_Ang);
}

void Can::CanConfigure(){
        can = socket(PF_CAN, SOCK_RAW, CAN_RAW); //create socket
        strcpy(ifr.ifr_name, "can0" );
        ioctl(can, SIOCGIFINDEX, &ifr); //select "can0"
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        bind(can, (struct sockaddr *)&addr, sizeof(addr)); // bind socket and "can0"

        rfilter[0].can_id = 0x600;// setting rules and monitor ID 0x600
        rfilter[0].can_mask = CAN_SFF_MASK;
        rfilter[1].can_id = 0x701;// setting rules and monitor ID 0x701
        rfilter[1].can_mask = CAN_SFF_MASK; // ID is 11 bits
        rfilter[2].can_id = 0x702;// setting rules and monitor ID 0x702
        rfilter[2].can_mask = CAN_SFF_MASK;
        setsockopt(can, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

}

Can::Can(int argc, char** argv)
{

    CanConfigure();
    TcpConfigure(argc,argv);
    RadarRecord.open("RadarData.txt");

}

Can::~Can(){
    RadarRecord.close();
    close(can);
}

