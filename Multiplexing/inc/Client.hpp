
#include <sys/socket.h>
#include <netinet/in.h>
#include "../../Request/request.hpp"
#include "../../reponse/reponse.hpp"

class Request;
class Reponse;

class Client {
    private:
        int client_fd;
        struct sockaddr_in client_address;
    public:
        Client();
        ~Client();
        int get_fd() const;
        struct sockaddr_in get_address() const;
        Request get_request() const;
        void set_fd(int fd);
        Request req;
        Response res;
};