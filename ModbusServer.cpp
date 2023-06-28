#include <iostream>
#include <numeric>

#include "ModbusServer.h"
#include "ModbusConnection.h"

#define NB_CONNECTION 5

ModbusServer::ModbusServer(int port, IGlobalService *server)
:port_(port)
,BaseServer(server)
{
}

ModbusServer::~ModbusServer()
{
    if (mb_mapping_ != nullptr)
        modbus_mapping_free(mb_mapping_);
    if (ctx_ != nullptr)
        modbus_free(ctx_);
}

void ModbusServer::run__()
{
    ctx_ = modbus_new_tcp(nullptr, 1502);
    if (ctx_ == nullptr)
    {
        std::cout << "failed to create modbus context. exiting" << "\n";
        return;
    }
    mb_mapping_ = modbus_mapping_new(10, 10, 10, 10);
    if (mb_mapping_ == nullptr)
    {
        std::cout << "failed to create modbus mapping. exiting" << "\n";
        return;
    }
    std::iota(mb_mapping_->tab_input_registers, mb_mapping_->tab_input_registers + mb_mapping_->nb_input_registers, 1);
    std::fill(mb_mapping_->tab_input_bits, mb_mapping_->tab_input_bits + mb_mapping_->nb_input_bits / 2, 1);
    server_socket_ = modbus_tcp_listen(ctx_, NB_CONNECTION);
    if (server_socket_ == -1)
    {
        std::cout << "Unable to listen TCP connection. exiting" << "\n";
        return;
    }
    while(bRunning)
    {
        struct sockaddr_in clientaddr{0};
        socklen_t addrlen = sizeof(clientaddr);
        int newSock = accept(server_socket_, (struct sockaddr *) &clientaddr, &addrlen);
        if (newSock != -1)
        {
            auto newConn = new ModbusConnection(newSock, clientaddr, this, this);
            registerConnection(newConn);
            newConn->run();
        }
    }
}

void ModbusServer::stop__()
{
    close(server_socket_);
    cancelIO();
    stopConnections();
}

int ModbusServer::handleModbusRequest(int sock)
{
    std::lock_guard<std::mutex> guard(ctxMutex);
    modbus_set_socket(ctx_, sock);
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc = modbus_receive(ctx_, query);
    if (rc > 0)
    {
        int n = modbus_reply(ctx_, query, rc, mb_mapping_);
    }
    return rc;
}

int ModbusServer::readCoil(int arg, std::uint8_t &val)
{
    std::lock_guard<std::mutex> guard(ctxMutex);
    if (arg >= nb_bits || arg < 0)
        return 0;
    val = mb_mapping_->tab_bits[arg];
    return 1;
}

int ModbusServer::readInput(int arg, std::uint16_t &val)
{
    std::lock_guard<std::mutex> guard(ctxMutex);
    if (arg >= nb_input_registers || arg < 0)
        return 0;
    val = mb_mapping_->tab_input_registers[arg];
    return 1;
}

int ModbusServer::readDiscrete(int arg, std::uint8_t &val)
{
    std::lock_guard<std::mutex> guard(ctxMutex);
    if (arg >= nb_input_bits || arg < 0)
        return 0;
    val = mb_mapping_->tab_input_bits[arg];
    return 1;
}

int ModbusServer::readHolding(int arg, std::uint16_t &val)
{
    std::lock_guard<std::mutex> guard(ctxMutex);
    if (arg >= nb_registers || arg < 0)
        return 0;
    val = mb_mapping_->tab_registers[arg];
    return 1;
}

int ModbusServer::readCoilAll(std::vector<uint8_t> &val)
{
    val.clear();
    std::lock_guard<std::mutex> guard(ctxMutex);
    if (nb_bits == 0)
        return 0;
    val.reserve(mb_mapping_->nb_bits);
    std::copy(mb_mapping_->tab_bits, mb_mapping_->tab_bits + mb_mapping_->nb_bits, std::back_inserter(val));
    return 1;
}

int ModbusServer::readInputAll(std::vector<uint16_t> &val)
{
    val.clear();
    std::lock_guard<std::mutex> guard(ctxMutex);
    if (nb_input_registers == 0)
        return 0;
    val.reserve(mb_mapping_->nb_input_registers);
    std::copy(mb_mapping_->tab_input_registers, mb_mapping_->tab_input_registers + mb_mapping_->nb_input_registers, std::back_inserter(val));
    return 1;
}

int ModbusServer::readDiscreteAll(std::vector<uint8_t> &val)
{
    val.clear();
    std::lock_guard<std::mutex> guard(ctxMutex);
    if (nb_input_bits == 0)
        return 0;
    val.reserve(mb_mapping_->nb_input_bits);
    std::copy(mb_mapping_->tab_input_bits, mb_mapping_->tab_input_bits + mb_mapping_->nb_input_bits, std::back_inserter(val));
    return 1;
}

int ModbusServer::readHoldingAll(std::vector<uint16_t> &val)
{
    val.clear();
    std::lock_guard<std::mutex> guard(ctxMutex);
    if (nb_registers == 0)
        return 0;
    val.reserve(mb_mapping_->nb_registers);
    std::copy(mb_mapping_->tab_registers, mb_mapping_->tab_registers + mb_mapping_->nb_registers, std::back_inserter(val));
    return 1;
}

int ModbusServer::readAll(allRegs &val)
{
    int n1 = readCoilAll(val.coils);
    int n2 = readInputAll(val.inputs);
    int n3 = readDiscreteAll(val.discrete);
    int n4 = readHoldingAll(val.holding);
    if (n1 > 0 && n2 > 0 && n3 > 0 && n4 > 0)
        return 1;
    else
        return 0;
}
