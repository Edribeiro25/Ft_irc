#include"Client.hpp"
#include"Server.hpp"

bool stop = false;

void handler(int) { stop = true; } 

int main(int ac,char **av)
{
    if(ac != 3)
    {
        std::cout << "Error : ./ircserv PORT PASS" << std::endl;
        return (0);
    }
    Server irc;
    
    irc.init(av[2],av[1]);
    signal(SIGINT, handler);
    while (stop == false)
    {
        irc.exec();
    }
    return(0);
}