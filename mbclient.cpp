#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "protocol.h"

using boost::asio::ip::tcp;

boost::asio::io_context io_context;
tcp::socket s(io_context);

void SplitString(const char *s, char c, std::vector<std::string>&v)
{
    v.clear();
    const char *p1 = s;
    for (const char *p2 = s; ; p2++)
    {
        if (*p2 == c)
        {
            std::string s1(p1, p2);
            v.push_back(s1);
            p1 = p2 + 1;
        }
        else if (*p2 == 0)
        {
            std::string s1(p1, p2);
            v.push_back(s1);
            break;
        }
    }
}

void printArgs(const std::vector<std::string> &args)
{
    for (int j = 0; j < args.size(); j++)
    {
        std::cout << "args[" << j << "] = " << args[j] << "\n";
    }
}

void connectToServer(const std::vector<std::string> &args)
{
    boost::system::error_code ec;
    tcp::resolver resolver(io_context);
    boost::asio::connect(s, resolver.resolve(args[1], args[2]), ec);
    if (ec)
    {
        std::cout << "error. " << ec.message() << "\n";
    }
    else
    {
        std::cout << "connected." << "\n";
    }
}

void disconnectFromServer(const std::vector<std::string> &args)
{
    boost::system::error_code ec;
    tcp::resolver resolver(io_context);
    s.close(ec);
    if (ec)
    {
        std::cout << "error. " << ec.message() << "\n";
    }
    else
    {
        std::cout << "disconnected." << "\n";
    }
}

void readResponse(std::vector<uint8_t> &response)
{
    boost::system::error_code ec;
    response.clear();
    std::uint32_t n;
    boost::asio::read(s, boost::asio::buffer(&n, 4), ec);
    if (ec)
    {
        return;
    }
    n = ntohl(n);
    response.resize(n);
    boost::asio::read(s, boost::asio::buffer(response), ec);
}

void decodeResponse(const std::vector<uint8_t> &response, std::vector<uint8_t> &data)
{
    data = response;
}

void decodeResponse(const std::vector<uint8_t> &response, uint8_t &data)
{
    data = response[0];
}

void decodeResponse(const std::vector<uint8_t> &response, uint16_t &data)
{
    uint16_t *ptr = (uint16_t *)response.data();
    data = ntohs(ptr[0]);
}

void decodeResponse(const std::vector<uint8_t> &response, std::vector<uint16_t> &data)
{
    uint16_t *ptr = (uint16_t *)response.data();
    data.resize(response.size()/2);
    for (int j = 0; j < data.size(); j++)
    {
        data[j] = ntohs(ptr[j]);
    }
}

void sendCommand(std::uint8_t cmd, std::uint16_t arg, std::vector<uint8_t> &response)
{
    std::uint16_t n = htons(arg);
    boost::asio::write(s, boost::asio::buffer(&cmd, 1));
    boost::asio::write(s, boost::asio::buffer(&n, 2));
    readResponse(response);
}

void readAllRegs()
{
    std::cout << "readAllRegs" << "\n";
    std::vector<uint8_t> response;
    sendCommand(READ_ALL, 0, response);
    if (response.size() > 0)
    {
        std::cout << "readDiscreteAll: response.size() = " << response.size() << "\n";
    }
    else
    {
        std::cout << "no values" << "\n";
    }
}

void readDiscreteAll()
{
    std::cout << "readDiscreteAll" << "\n";
    std::vector<uint8_t> response;
    sendCommand(READ_DISCRETE_ALL, 0, response);
    if (response.size() > 0)
    {
        std::cout << "readDiscreteAll: response.size() = " << response.size() << "\n";
        std::vector<uint8_t> regs;
        decodeResponse(response, regs);
        for (int j = 0; j < regs.size(); j++)
        {
            std::cout << "value[" << j << "] = " << (uint)regs[j] << "\n";
        }
    }
    else
    {
        std::cout << "no values" << "\n";
    }
}

void readCoilAll()
{
    std::vector<uint8_t> response;
    sendCommand(READ_COIL_ALL, 0, response);
    if (response.size() > 0)
    {
        std::vector<uint8_t> coils;
        decodeResponse(response, coils);
        for (int j = 0; j < coils.size(); j++)
        {
            std::cout << "value[" << j << "] = " << (uint)coils[j] << "\n";
        }
    }
    else
    {
        std::cout << "no values" << "\n";
    }
}

void readInputAll()
{
    std::vector<uint8_t> response;
    sendCommand(READ_INPUT_ALL, 0, response);
    if (response.size() > 0)
    {
        std::vector<uint16_t> inputs;
        decodeResponse(response, inputs);
        for (int j = 0; j < inputs.size(); j++)
        {
            std::cout << "value[" << j << "] = " << (uint)inputs[j] << "\n";
        }
    }
    else
    {
        std::cout << "no values" << "\n";
    }
}

void readHoldingAll()
{
    std::cout << "readHoldingAll" << "\n";
    std::vector<uint8_t> response;
    sendCommand(READ_HOLDING_ALL, 0, response);
    if (response.size() > 0)
    {
        std::cout << "readHoldingAll: response.size() = " << response.size() << "\n";
        std::vector<uint16_t> regs;
        decodeResponse(response, regs);
        for (int j = 0; j < regs.size(); j++)
        {
            std::cout << "value[" << j << "] = " << (uint)regs[j] << "\n";
        }
    }
    else
    {
        std::cout << "no values" << "\n";
    }
}

void readDiscrete(int n)
{
    std::vector<uint8_t> response;
    sendCommand(READ_DISCRETE, n, response);
    if (response.size() > 0)
    {
        uint8_t data;
        decodeResponse(response, data);
        std::cout << "value = " << (uint)data << "\n";
    }
    else
    {
        std::cout << "no values" << "\n";
    }
}

void readCoil(int n)
{
    std::vector<uint8_t> response;
    sendCommand(READ_COIL, n, response);
    if (response.size() > 0)
    {
        uint8_t data;
        decodeResponse(response, data);
        std::cout << "value = " << (uint)data << "\n";
    }
    else
    {
        std::cout << "no values" << "\n";
    }
}

void readInput(int n)
{
    std::vector<uint8_t> response;
    sendCommand(READ_INPUT, n, response);
    if (response.size() >= 2)
    {
        uint16_t data;
        decodeResponse(response, data);
        std::cout << "value = " << data << "\n";
    }
    else
    {
        std::cout << "no values" << "\n";
    }
}

void readHolding(int n)
{
    std::vector<uint8_t> response;
    sendCommand(READ_HOLDING, n, response);
    if (response.size() >= 2)
    {
        uint16_t data;
        decodeResponse(response, data);
        std::cout << "value = " << data << "\n";
    }
    else
    {
        std::cout << "no values" << "\n";
    }
}

void readAllRegsByType(const std::string &mode)
{
    if (mode == "discrete")
    {
        readDiscreteAll();
    }
    else if (mode == "coil")
    {
        readCoilAll();
    }
    else if (mode == "input")
    {
        readInputAll();
    }
    else if (mode == "holding")
    {
        readHoldingAll();
    }
    else
    {
        std::cout << "invalid argument" << "\n";
    }
}

void readRegByType(const std::string &mode, int n)
{
    if (mode == "discrete")
    {
        readDiscrete(n);
    }
    else if (mode == "coil")
    {
        readCoil(n);
    }
    else if (mode == "input")
    {
        readInput(n);
    }
    else if (mode == "holding")
    {
        readHolding(n);
    }
    else
    {
        std::cout << "invalid argument" << "\n";
    }
}

/*
1. считать цифровой вход (номер входа), (discrete input - 02) command: read discrete n
2. считать цифровой выход (номер выхода), (coil - 01) command: read coil n
3. считать входной регистр (номер регистра), (input - 04) command: read input n
4. считать выходной регистр (номер регистра), (holding - 03) command: read holding n
where n is number of register. In this demo only 0...9 are supported
5. считать все входа, all 1 command: read discrete all
6. считать все выхода, all 2
7. считать все входные регистры, all 3
8. считать все выходные регистры, all 4
9. считать все данные. 5+6+7+8 command: read all
 */
void processReadOp(const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << "too few args" << "\n";
        return;
    }
    std::string mode = args[1];
    if (mode == "all")
    {
        readAllRegs();
    }
    else
    {
        if (args.size() < 3)
        {
            std::cout << "too few args" << "\n";
            return;
        }
        std::string arg = args[2];
        if (arg == "all")
        {
            readAllRegsByType(mode);
        }
        else
        {
            int n = std::stoi(arg);
            readRegByType(mode, n);
        }
    }
}

void startLoop(const std::vector<std::string> &args)
{
    std::cout << "startLoop" << "\n";
    printArgs(args);
}

void stopLoop(const std::vector<std::string> &args)
{
    std::cout << "stopLoop" << "\n";
    printArgs(args);
}

bool processCommand(std::string command)
{
    if (command.empty())
        return false;

    std::vector<std::string> sv;
    SplitString(command.c_str(), ' ', sv);
    if (sv[0] == "connect")
        connectToServer(sv);
    else if (sv[0] == "disconnect")
        disconnectFromServer(sv);
    else if (sv[0] == "read")
        processReadOp(sv);
    else if (sv[0] == "start")
        startLoop(sv);
    else if (sv[0] == "stop")
        stopLoop(sv);
    else
        std::cout << "Invalid command" << "\n";
    return true;
}

int main(int ac, char *av[])
{
    std::string command;
    while (true)
    {
        std::cout << "Enter command (empty command to quit): ";
        std::getline(std::cin, command);
        if (!processCommand(command))
            break;
    }
    return 0;
}
