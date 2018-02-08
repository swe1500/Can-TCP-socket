#include <iostream>
#include "cant.h"


int main(int argc, char** argv)
{
    Can can(argc, argv);
    printf("Start to send message\n");
    printf("message form: ID\n");
    while(1){
        can.Update();
        can.SendMessage();
    }
    return 0;
}

