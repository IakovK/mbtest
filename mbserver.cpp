#include <iostream>
#include <functional>
#include "ThreadDeleter.h"
#include "ModbusServer.h"
#include "TcpServer.h"

std::function<void()> handler;
void signalHandler(int signal)
{
    (void) signal;
    if (handler)
        handler();
}

int main(int ac, char *av[])
{
    int modbusPort = 1502;   // port at which modbus server will listen
    int tcpPort = 5000;     // port at which tcp server will listen

    ThreadDeleter td;
    ModbusServer s1(modbusPort, &td);
    TcpServer s2(tcpPort, &td, &s1);
    handler = [&]
    {
        s1.stop();
        s2.stop();
    };
    struct sigaction newAction{0};
    (void)sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;
    newAction.sa_handler = signalHandler;
    (void)sigaction(SIGINT, &newAction, NULL);
    s1.run();
    s2.run();
    td.run();
    s1.wait();
    s2.wait();
    td.stop();
    td.wait();
    return 0;
}
