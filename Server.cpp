#include "Server.hpp"

Server::Server() : _msg(""){}

Server::~Server()
{
	for(std::vector<Client *>::iterator itc = _client.begin(); itc != _client.end(); ++itc)
	{
		delete((*itc));
	}
	for(std::vector<Channel *>::iterator itc = _chan.begin(); itc != _chan.end(); ++itc)
	{
		delete((*itc));
	}
	close(fds[0].fd);
}

void Server::error(std::string err)
{ 
	std::cout << "Error : " << err << std::endl;
	exit(1); 
}

void Server::exec()
{

	if (poll(&fds[0], fds.size(), (6000)) == -1)
	{
		std::cout << "poll -1 "<< std::endl;
		return;
	}
	if (fds[0].revents == POLLIN) 
		Newclient(); 
	else
	{
		for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it)
		{
			if ((*it).revents == POLLIN)
			{
				for(std::vector<Client *>::iterator itc = _client.begin(); itc != _client.end(); ++itc)
				{
					if((*it).fd == (*itc)->getSocket())
						(*itc)->receive();
					if ((*itc)->getMsgcomplete() == 1)
					{
						Command cmd((*itc), this);
					}
				}
			}
		}
		for(std::vector<Client *>::iterator itc = _client.begin(); itc != _client.end(); ++itc)
		{
			if((*itc)->getStatus() == DELETE)
			{
				_toremove.push_back((*itc)->getSocket());
			}
		}
		for (std::vector<int>::iterator it = _toremove.begin(); it != _toremove.end(); ++it)
		{
			for (std::vector<pollfd>::iterator it1 = fds.begin(); it1 != fds.end(); ++it1)
			{
				if (*it == (*it1).fd)
				{
					fds.erase(it1);
					it1 = fds.end() - 1;
				}
			}
			for(std::vector<Client *>::iterator itc = _client.begin(); itc != _client.end(); ++itc)
			{

				if (*it == (*itc)->getSocket())
				{
					delete((*itc));
					_client.erase(itc);
					itc = _client.end() - 1;
				}
			}
		}
		_toremove.clear();
	}
}


void Server::Newclient(void)
{
	int fd;
	struct sockaddr_in addressc; 
	socklen_t csize = sizeof(addressc);
	fd = accept(fds[0].fd,(struct sockaddr *)&addressc , &csize);
	if (fd == -1)
		error("error accept");
	fds.push_back(pollfd());
	fds.back().fd = fd;
	fds.back().events = POLLIN;
	_client.push_back(new Client);
	_client.back()->setSocket(fd);
	std::cout << "New connection with fd :" << fd << std::endl;
}

void Server::init(std::string password,std::string port)
{
	int enable = 1;
	int _fd;
	if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		error("socket");
	else
		std::cout << "socket ok "<< std::endl;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable))) 
		error("setsockopt");
	else
		std::cout << "setsockopt ok "<< std::endl;
	struct sockaddr_in address; 
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(atoi(port.c_str()));
	if (bind(_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
		error("bind");
	else
		std::cout << "Bind ok"<< std::endl;
	if (listen(_fd, MAX_USER) < 0)
		error("listen ");
	else
		std::cout << "Listen ok"<< std::endl;
	fds.push_back(pollfd());
	fds.back().fd = _fd;
	fds.back().events = POLLIN;
	_pass = password;
	_port = port;
	std::cout << "PASS: " << password << " PORT: " << port << std::endl;
}

bool Server::isachan(std::string name)
{
	for (std::vector<Channel *>::iterator it = _chan.begin(); it < _chan.end() ; ++it)
	{
		if((*it)->getName() == name)
			return(true);
	}
	return(false);
}


bool Server::isaclient(std::string name)
{
	for (std::vector<Client *>::iterator it = _client.begin(); it < _client.end() ; ++it)
	{
		if((*it)->getNickname() == name)
			return(true);
	}
	return(false);
}

void Server::Newchan(std::string name,int op)
{
	Channel *chan = new Channel(name,op);
	_chan.push_back(chan);
}

Channel *Server::findChan(std::string name)
{
	if (_chan.empty())
		return (NULL);
	for (std::vector<Channel *>::iterator it = (_chan).begin() ; it != (_chan).end() ; ++it)
		if ((*it)->getName() == name)
			return ((*it));
	return (NULL);
}

Client *Server::findClient(std::string name)
{
	if (_client.empty())
		return (NULL);
	for (std::vector<Client *>::iterator it = (_client).begin() ; it != (_client).end() ; ++it)
	{
		if ((*it)->getNickname() == name)
			return ((*it));
	}
	return (NULL);
}

void	Server::DelChan(std::string name)
{
	for (std::vector<Channel *>::iterator it = (_chan).begin() ; it != (_chan).end() ; ++it)
	{
		if ((*it)->getName() == name)
		{
			delete(*it);
			_chan.erase(it);
			it = _chan.end() - 1;
		}
	}
}
void	Server::Deluser(Client *clt)
{
	int fd = clt->getSocket();
	for (std::vector<pollfd>::iterator it1 = fds.begin(); it1 != fds.end(); ++it1)
	{
		if (fd == (*it1).fd)
		{
			close(fd);
			fds.erase(it1);
			it1 = fds.end() - 1;
		}
	}
	for(std::vector<Client *>::iterator itc = _client.begin(); itc != _client.end(); ++itc)
	{
		if (fd == (*itc)->getSocket())
		{
			delete((*itc));
			_client.erase(itc);
			itc = _client.end() - 1;
		}
	}
}

std::vector<Client *>       Server::getClientList(){return (_client);}
std::vector<Channel *>      Server::getChanList(){return (_chan);}
std::vector<int>    		Server::getToremove(){return (_toremove);}
std::string					Server::getPass(){return (_pass);}
