#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <netinet/in.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cstdlib>
#include "Client.hpp"
#include "Command.hpp"

#define MAX_USER 3
#define OPER "admin"
#define OPERPWD "lol"

class Server
{
    public:
            Server(/* args */);
            ~Server();
            void exec();
            void init(std::string password,std::string port);
            void error(std::string err);
            
            void Newclient(void);
            void Deluser(Client *clt);

            void Newchan(std::string name, int op);
            void DelChan(std::string name);

        
            Client *findClient(std::string name);
            Channel *findChan(std::string name);

            std::vector<Client *>       getClientList();
            std::vector<Channel *>      getChanList();
            std::string                 getPass();
            std::vector<int>            getToremove();
            std::vector<pollfd>         fds;

            bool isachan(std::string name);
            bool isaclient(std::string name);

    private:
            std::vector<Client *> _client;
            std::vector<Channel *> _chan;
            std::vector<int> _toremove;
            std::string _msg;
            std::string _pass;
            std::string _port;
};

#endif