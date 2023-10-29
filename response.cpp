#include "Multiplexing/inc/Multiplexing.hpp"
#include "Multiplexing/inc/Socket.hpp"

void Multiplexing::send_res(int fd)
{
    std::ostringstream contentLengthHeader;
    std::string response;
    std::ifstream file;
    std::string temp;
    int file_size;
    std::string type = "text/html";
    std::string file_name = "root/index.html";
    std::string  line;

        file.open(file_name.c_str());
        if (!file.is_open() && !file_name.empty())
        {
            std::cout << "Error opening file " << file_name << std::endl;
            return;
        }


        file.seekg(0, std::ios::end);
        file_size = file.tellg();
        file.seekg(0, std::ios::beg);

         while (std::getline(file, line)) {
      //  std::cout << line << std::endl;

            temp += line;
        
    }
            response = "HTTP/1.1 200 OK\r\nContent-Type: " + type + "\r\n";
            contentLengthHeader << "Content-Length: " << file_size << "\r\n\r\n";
            response += contentLengthHeader.str();
        
        std::cout << "sending.." << std::endl;
        
     //  std::cout << response.c_str() << std::endl;
        if (send(fd, response.c_str(), response.size(), 0) < 1)
        {
            std::cout << "Error sending response header" << std::endl;
            file.close();
            return;
        }

        if (send(fd, temp.c_str(), file_size, 0) < 1)
             std::cout << "Error sending file" << std::endl;
}