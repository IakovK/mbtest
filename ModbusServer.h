#pragma once

#include "Worker.h"
#include "BaseServer.h"

#include <modbus.h>

class ModbusServer: public Worker, public BaseServer, public IModbusService
{
    int port_;
    void run__() override;
    void stop__() override;

    modbus_t *ctx_{nullptr};
    modbus_mapping_t *mb_mapping_{nullptr};
    std::mutex ctxMutex;

    int server_socket_{-1};

public:
    static const int nb_bits{10};
    static const int nb_input_bits{10};
    static const int nb_registers{10};
    static const int nb_input_registers{10};
    ModbusServer(int port, IGlobalService *server);
    ~ModbusServer();
    int handleModbusRequest(int sock) override;
    int readCoil(int arg, std::uint8_t &val) override;
    int readInput(int arg, std::uint16_t &val) override;
    int readDiscrete(int arg, std::uint8_t &val) override;
    int readHolding(int arg, std::uint16_t &val) override;
};
