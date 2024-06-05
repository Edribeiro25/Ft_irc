#include "Client.hpp"

Client::Client() : _status(TO_REGISTER), _msg_complete(0), _oper(0)
{

}
Client::~Client() {close(_socket);}

int Client::getSocket() {return(_socket);}
userStatus Client::getStatus() {return(_status);}
std::string Client::getHostname() {return(_hostname);}
std::string Client::getNickname() {return(_nickname);}
std::string Client::getUsername() {return(_username);}
std::string Client::getRealname() {return(_realname);}
void        Client::setSocket(int fd) {_socket = fd;}
void        Client::setHostname(std::string hostname) {_hostname = hostname;}
void        Client::setNickname(std::string nickname) {_nickname = nickname;}
void        Client::setUsername(std::string username) {_username = username;}
void        Client::setRealname(std::string realname) {_realname = realname;}
void        Client::setStatus(userStatus status) {_status = status;}
void        Client::setOper(bool oper) {_oper = oper;}
bool        Client::getOper() {return (_oper);}
bool        Client::getMsgcomplete() {return(_msg_complete);}
void        Client::setServername(std::string servername) {_server_name = servername;}
std::string Client::getServername() {return (_server_name);}
std::vector<std::vector<std::string> > *Client::getCommand() {return (&_cmd);}
std::vector<Channel *>  Client::getClientChannelsList() {return (_client_channels_list);}


void        Client::receive()
{
    char buffer[MAX + 1];
    memset(buffer, 0, MAX);
    if (_msg_complete)
    {
        _msg.clear();
        _msg_complete = 0;
    }
    ssize_t size;
    if ((size = recv(_socket, &buffer, MAX, 0)) == -1)
    {
        _msg.clear();
        return;
    }
    if (size == 0)
    {
        _msg.clear();
        _status = DELETE;
        return;
    }
    buffer[size] = 0;
    _msg += buffer;
    //std::cout << "MSG RECU : " << _msg;
    if (_msg.size() >= 2 && *(_msg.end() - 2) == '\r' && *(_msg.end() - 1) == '\n')
    {
        _msg_complete = 1;
        split_command();
    }
    else    
		_msg_complete = 0;
}

void    Client::split_command()
{
    int i = 0;
	int new_start = 0;
	int number_cmd = 0;
	while (_msg[i] && _msg[i+1])
	{
		if (_msg[i] ==  '\r' && _msg[i + 1] == '\n')
		{
			std::string tmp_cmd(_msg.substr(new_start, i - new_start));
			new_start = i + 2;
			i += 2;
			int j = 0;
			int split_start = 0;
			int special_case = 0;
			while (tmp_cmd[j])
			{
				if (tmp_cmd[j] == ' ' || !tmp_cmd[j + 1])
				{
					if (!tmp_cmd[j + 1] && tmp_cmd[j] != ' ')
						special_case = 1;
					std::string split_split(tmp_cmd.substr(split_start, (j + special_case) - split_start));
					_cmd.push_back(std::vector<std::string>());
					_cmd[number_cmd].push_back(split_split);
					split_start = j + 1;
				}
				j++;
			}
			number_cmd++;
		}
		else
			i++;
	}
}

 void        Client::clear_command()
 {
     for (std::vector<std::vector<std::string> >::iterator it =_cmd.begin() ; it != _cmd.end() ; ++it)
     {
        for (std::vector<std::string>::iterator it2 = (*it).begin() ; it2 != (*it).end() ; ++it2)
        {
            (*it2).erase();
        }
        (*it).clear();
     }
 }

 void        Client::leaveChan(Channel * chan)
 {
    for (std::vector<Channel *>::iterator it = _client_channels_list.begin() ; it != _client_channels_list.end() ; ++it)
    {
        if ((*it) == chan)
        {
            _client_channels_list.erase(it);
            return ;
        }
    }
 }

 void        Client::leaveAllChan()
 {
    _client_channels_list.clear();
 }
 
 void        Client::addchan(Channel *chan)
 {
    _client_channels_list.push_back(chan);
 }

 void        Client::remove_client_from_channel(Client *client)
 {
    for (std::vector<Channel *>::iterator it = _client_channels_list.begin() ; it != _client_channels_list.end() ; ++it)
    {
        (*it)->removeclient(client->getSocket());
    }
 }