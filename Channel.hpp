#ifndef CHANNEL_HPP
#define CHANNEL_HPP


#include "Client.hpp"
#include "Server.hpp"

class Client;

class Channel
{
private:
    std::string _name;
    std::string _topic;

    std::vector<Client *>   _cltinv;
    std::vector<Client *>   _clt;
    int                     _op;
    int                     _nbclt;

public:
    Channel(std::string name, int op);
    ~Channel();

    void setName(std::string name);
    void setTopic(std::string topic);
 
    std::string getName();
    std::string getTopic();
    std::vector<Client *>   getClientsList();
    Client *findClt(std::string name);
    bool topicisset();

    int     getchanop();
    int     getnbclt();
    void    setChanOp(bool op);

    void addclient(Client *clt);
    void removeclient(int fd);

    void addinv(Client *clt);
    void removeinv(int fd);
};

#endif