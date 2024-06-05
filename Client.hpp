#ifndef CLIENT_HPP
#define CLIENT_HPP

#define MAX 512
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include "Channel.hpp"

class Channel;

enum userStatus
{
    TO_REGISTER,
    PASSWORD,
    REGISTERED,
    DELETE
};

class Client
{
    public:
            Client();
            ~Client();
            std::string getHostname();
            std::string getNickname();
            std::string getUsername();
            std::string getRealname();
            void        setHostname(std::string hostname);
            void        setNickname(std::string nickname);
            void        setUsername(std::string username);
            void        setRealname(std::string realname);
            userStatus  getStatus();
            void        setStatus(userStatus status);
            int         getSocket();
            void		setServername(std::string servername);
		    std::string	getServername();
            void        receive();
            void        split_command();
            bool        getMsgcomplete();
            void        setSocket(int fd);
            void        setOper(bool oper);
            bool        getOper();
            std::vector<std::vector<std::string> > *getCommand();
            void        clear_command();
            std::vector<Channel *>  getClientChannelsList();
            void        leaveChan(Channel * chan);
            void        leaveAllChan();
            void        addchan(Channel *clt);
            void        remove_client_from_channel(Client *client);

    private:
            int         _socket;

            std::string _hostname;
            std::string _nickname;
            std::string _username;
            std::string _realname;
            std::string	_server_name;

            std::string _msg;
            userStatus  _status;
            bool        _msg_complete;

            bool        _oper;

            std::vector<std::vector<std::string> > _cmd;

            std::vector<Channel *>  _client_channels_list;

};


#endif