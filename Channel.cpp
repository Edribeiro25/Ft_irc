#include "Channel.hpp"

Channel::Channel(std::string name,int op): _topic("") , _op(op)
{
    this->setName(name);
    _nbclt = 0;
}

Channel::~Channel()
{
}

void Channel::setName(std::string name)
{
    this->_name = name;
}

bool Channel::topicisset()
{
    if (_topic != "")
        return(true);
    return(false);    
}

void Channel::setTopic(std::string topic)
{
    this->_topic = topic;
}

std::string Channel::getName(void)
{
    return (_name);
}

std::string Channel::getTopic(void)
{
    return (_topic);
}

int     Channel::getnbclt(void)
{
    return(_nbclt);
}

void    Channel::setChanOp(bool op)
{
    _op = op;
}

int     Channel::getchanop()
{
    return (_op);
}

std::vector<Client *> Channel::getClientsList()
{
    return (_clt);
}

Client *Channel::findClt(std::string name)
{
	if (_clt.empty())
		return (NULL);
	for (std::vector<Client *>::iterator it = (_clt).begin() ; it != (_clt).end() ; ++it)
	{
		if ((*it)->getNickname() == name)
			return ((*it));
	}
	return (NULL);
}

void Channel::addclient(Client *clt)
{
    _clt.push_back(clt);
    _nbclt++;
}

void Channel::removeclient(int fd)
{
    if(fd == _op)
    {
        _op = 0;
    }
    for (std::vector<Client *>::iterator it = _clt.begin() ; it < _clt.end(); ++it)
    {
        if((*it)->getSocket() == fd)
        {
            _clt.erase(it);
            it = _clt.end() - 1;
            _nbclt--;
        }
    }
}

void Channel::addinv(Client *clt)
{
    _cltinv.push_back(clt);
}

void Channel::removeinv(int fd)
{
    for (std::vector<Client *>::iterator it = _clt.begin() ; it < _clt.end(); ++it)
    {
        if((*it)->getSocket() == fd)
        {
            _cltinv.erase(it);
            it = _cltinv.end() - 1;
        }
    }
}