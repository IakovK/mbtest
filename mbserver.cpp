#include <iostream>
#include <thread>
#include <functional>
#include <mutex>

#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <modbus.h>

#define NB_CONNECTION 5

bool bRunning{false};   // this global variable will be used to control execution of the servers. Initially true, it will be set to false to signal to the servers to terminate;

class Worker
{
    static void run_(Worker *this_);
protected:
    std::thread thread_;
    virtual void run__() = 0;
public:
    void run();
    void wait();
    virtual ~Worker(){}
};

class IService
{
public:
    virtual void registerConnection(Worker *w) = 0;
    virtual void deregisterConnection(Worker *w) = 0;
    virtual int handleModbusRequest(int sock) = 0;
};

class ModbusServer: public Worker, public IService
{
    int port_;
    void run__() override;
    modbus_t *ctx_{nullptr};
    std::mutex ctxMutex;
    modbus_mapping_t *mb_mapping_{nullptr};
    int server_socket_{-1};
    void cancelIO();

public:
    ModbusServer(int port);
    ~ModbusServer();
    void stop();
    void registerConnection(Worker *w) override;
    void deregisterConnection(Worker *w) override;
    int handleModbusRequest(int sock) override;
};

class ModbusConnection: public Worker
{
    int socket_;
    sockaddr_in clientaddr_;
    IService *server_;
    void run__() override;
public:
    ModbusConnection(int socket, sockaddr_in clientaddr, IService *server);
    ~ModbusConnection();
};

ModbusConnection::ModbusConnection(int socket, sockaddr_in clientaddr, IService *server)
:socket_(socket)
,clientaddr_(clientaddr)
,server_(server)
{
    std::cout << "ModbusConnection::ModbusConnection" << "\n";
}

ModbusConnection::~ModbusConnection()
{
    std::cout << "ModbusConnection::~ModbusConnection" << "\n";
}

void ModbusConnection::run__()
{
    std::cout << "New connection from " << inet_ntoa(clientaddr_.sin_addr) << ":" << clientaddr_.sin_port << "\n";
    // making socket nonblocking
    fcntl(socket_, F_SETFL, fcntl(socket_, F_GETFL, 0) | O_NONBLOCK);
    server_->registerConnection(this);
    while(bRunning)
    {
        fd_set rdset;
        FD_ZERO(&rdset);
        FD_SET(socket_, &rdset);
        std::cout << "ModbusConnection::run__: calling select" << "\n";
        int n = select(socket_ + 1, &rdset, NULL, NULL, NULL);
        std::cout << "ModbusConnection::run__: calling select completed: n = " << n << "\n";
        if (FD_ISSET(socket_, &rdset))
        {
            int n1 = server_->handleModbusRequest(socket_);
            std::cout << "ModbusConnection::run__: after handleModbusRequest(socket_): n1 = " << n1 << "\n";
            if (n1 < 0)
                break;
        }
    }
    std::cout << "ModbusConnection::run__: exiting" << "\n";
    std::cout << "Closed connection from " << inet_ntoa(clientaddr_.sin_addr) << ":" << clientaddr_.sin_port << "\n";
    server_->deregisterConnection(this);
}

class TcpServer: public Worker
{
    int port_;

    public:
        TcpServer(int port);
};

ModbusServer::ModbusServer(int port)
:port_(port)
{
}

ModbusServer::~ModbusServer()
{
    if (mb_mapping_ != nullptr)
        modbus_mapping_free(mb_mapping_);
    if (ctx_ != nullptr)
        modbus_free(ctx_);
}

void Worker::run()
{
    thread_ = std::thread(run_, this);
}

void Worker::run_(Worker *this_)
{
    this_->run__();
}

void ModbusServer::run__()
{
    std::cout << "ModbusServer::run__" << "\n";
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
        std::cout << "ModbusServer::run__: calling accept" << "\n";
        int newSock = accept(server_socket_, (struct sockaddr *) &clientaddr, &addrlen);
        if (newSock != -1)
        {
            std::cout << "ModbusServer::run__: newSock = " << newSock << "\n";
            auto newConn = new ModbusConnection(newSock, clientaddr, this);
            newConn->run();
        }
    }
}

static void noop(const int sig)
{
    std::cout << "noop" << "\n";
}

void ModbusServer::cancelIO()
{
    std::cout << "ModbusServer::cancelIO: entry" << "\n";
    struct sigaction newAction{0};
    (void)sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;
    newAction.sa_handler = noop;
    int n1 = sigaction(SIGUSR1, &newAction, NULL);
    std::cout << "ModbusServer::cancelIO: sigaction(SIGUSR1) returned " << n1 << "\n";
    int n2 = pthread_kill(thread_.native_handle(), SIGUSR1); // system calls return with EINTR
    std::cout << "ModbusServer::cancelIO: pthread_kill(SIGUSR1) returned " << n2 << "\n";
    std::cout << "ModbusServer::cancelIO: exit" << "\n";
}

void ModbusServer::stop()
{
    close(server_socket_);
    cancelIO();
}

void ModbusServer::registerConnection(Worker *w)
{
    std::cout << "ModbusServer::registerConnection" << "\n";
}

void ModbusServer::deregisterConnection(Worker *w)
{
    std::cout << "ModbusServer::deregisterConnection" << "\n";
}

int ModbusServer::handleModbusRequest(int sock)
{
    std::cout << "ModbusServer::handleModbusRequest" << "\n";
    std::lock_guard<std::mutex> guard(ctxMutex);
    modbus_set_socket(ctx_, sock);
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc = modbus_receive(ctx_, query);
    std::cout << "ModbusServer::handleModbusRequest: after modbus_receive: rc = " << rc << "\n";
    if (rc > 0)
    {
        int n = modbus_reply(ctx_, query, rc, mb_mapping_);
        std::cout << "ModbusServer::handleModbusRequest: after modbus_reply: n = " << n << "\n";
    }
    return rc;
}

void Worker::wait()
{
    std::cout << "Worker::wait: entry" << "\n";
    if (thread_.joinable())
    {
        std::cout << "Worker::wait: thread is joinable. calling join()" << "\n";
        thread_.join();
        std::cout << "Worker::wait: thread is joinable. calling join() complete" << "\n";
    }
    std::cout << "Worker::wait: exit" << "\n";
}

TcpServer::TcpServer(int port)
:port_(port)
{
}

std::function<void()> handler;
void signalHandler(int signal)
{
    (void) signal;
    bRunning = false;
    if (handler)
        handler();
}

int main(int ac, char *av[])
{
    int modbusPort = 1502;   // port at which modbus server will listen
    int tcpPort = 5000;     // port at which tcp server will listen

    bRunning = true;
    ModbusServer s1(modbusPort);
    //TcpServer s2(tcpPort);
    handler = [&]
    {
        s1.stop();
        //s2.stop();
    };
    struct sigaction newAction{0};
    (void)sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;
    newAction.sa_handler = signalHandler;
    (void)sigaction(SIGINT, &newAction, NULL);
    s1.run();
    //s2.run();
    std::cout << "calling s1.wait()" << "\n";
    s1.wait();
    std::cout << "calling s1.wait() done" << "\n";
    //s2.wait();
    std::cout << "exiting" << "\n";
    std::cout << "Press Enter to exit" << "\n";
    std::string s;
    std::getline(std::cin, s);
    return 0;
}
