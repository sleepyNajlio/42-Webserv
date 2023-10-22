
#include <sys/socket.h>
#include <netinet/in.h>
#include "../../request/request.hpp"
#include "../../Configue/includes/Server.hpp"
class Request;

class Client {
    private:
        int client_fd;
        struct sockaddr_in client_address;
    public:
        Client();
        ~Client();
        int get_fd() const;
        struct sockaddr_in get_address() const;
        void set_fd(int fd);
        Request request;

};