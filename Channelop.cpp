#include"Command.hpp"

void	Command::join()
{
	if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
	if (_client->getStatus() == TO_REGISTER)
	{
		sendToClient(451);
		return ;
	}
	if ((*_cmd)[_k].size() == 1)
	{
		sendToClient(461);
		return ;
	}
	std::vector<std::string> split_chan = splitCommas((*_cmd)[_k][1]);
	for (std::vector<std::string>::iterator it = split_chan.begin() ; it != split_chan.end() ; ++it)
		joinSingle(*it);
}
void	Command::joinSingle(std::string current_chan)
{
	if (current_chan[0] != '#') 
		return ;
	Channel *finded = _server->findChan(current_chan);
	if (!finded) 
	{
		_server->Newchan(current_chan,_client->getSocket());
		finded = _server->findChan(current_chan);
        finded->addclient(_client);
		_client->addchan(finded);
		_chan_name = finded;
	}
	else
	{
		finded->addclient(_client);
		_client->addchan(finded);
		_chan_name = finded;
	}
	std::string msg;
	msg =":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " " + "JOIN " + finded->getName() + "\r\n";
	std::vector<Client *> listClientsChan = finded->getClientsList();
	for (std::vector<Client *>::iterator it = listClientsChan.begin() ; it != listClientsChan.end() ; ++it)
	{
		send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
	}
	if (finded->getTopic() != "")
		sendToClient(332);
	nameReply(finded->getName(), finded);
}

void	Command::quitChan(std::string chan)
{
	Channel	*to_quit = _server->findChan(chan);
	if (!to_quit)
	{
		sendToClient(403);
		return ;
	}
	_chan_name = to_quit;
	std::vector<Client *>	clients_list = to_quit->getClientsList();
	for (std::vector<Client *>::iterator it = clients_list.begin() ; it != clients_list.end() ; ++it)
	{
		if ((*it)->getNickname() == _client->getNickname())
		{
			if (to_quit->getchanop() == _client->getSocket())
				to_quit->setChanOp(0);
			std::string	msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " PART " + to_quit->getName();
			if ((*_cmd)[_k].size() > 2)
			{	
				for (std::vector<std::string>::iterator it1 = (*_cmd)[_k].begin() + 2 ; it1 != (*_cmd)[_k].end() ; ++it1)
					msg += " " + *it1;
			}
			msg += "\r\n";
			for (std::vector<Client *>::iterator it2 = clients_list.begin() ; it2 != clients_list.end() ; ++it2)
				send((*it2)->getSocket(), msg.c_str(), msg.size(), 0);
			_client->leaveChan(to_quit);
			to_quit->removeclient(_client->getSocket());
			deleteEmptyChans();
			return ;
		}
	}
	sendToClient(442);
}

void	Command::part()
{
	if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
	if ((*_cmd)[_k].size() == 1)
	{
		sendToClient(461);
		return ;
	}
	std::vector<std::string> chan_list = splitCommas((*_cmd)[_k][1]);
	for (std::vector<std::string>::iterator it = chan_list.begin() ; it != chan_list.end() ; ++it)
		quitChan(*it);
}

void	Command::topic()
{
	if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
	{
		sendToClient(451);
		return ;
	}
	if ((*_cmd)[_k].size() == 1)
	{
		sendToClient(461);
		return ;
	}

	std::string chan_name = (*_cmd)[_k][1];
	Channel *finded = _server->findChan(chan_name);
	_badchan = (*_cmd)[_k][1];
	_chan_name = finded;
	if (!finded)
	{
		sendToClient(403);
		return ;
	}
	Client *user  = finded->findClt(_client->getNickname());
	if(!user)
	{
		sendToClient(442);
		return ;
	}
	if ((*_cmd)[_k].size() == 2) 
	{
		if (!(finded->topicisset()))
		{
			sendToClient(331);
			return ;
		}
		else
		{
			sendToClient(332);
			sendToClient(333); 
			return ;
		}
	}
	else if (finded->getchanop() != _client->getSocket())
	{
		sendToClient(482);
		return;
	}
	else if ((*_cmd)[_k].size() >= 3)
	{
	
		std::string topic;
		for(std::vector<std::string>::iterator it1 = (*_cmd)[_k].begin() + 2 ; it1 != (*_cmd)[_k].end() ; ++it1)
		{
			if((*it1) != ":")
				topic += (*it1);
		}
		if(topic == "::" || topic == ":")
			topic = "";
		else
			topic.erase(0,1);
		finded->setTopic(topic);
		std::string msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " TOPIC " + finded->getName() + " " + topic + "\r\n";
		std::vector<Client *> listClients = finded->getClientsList();
		for (std::vector<Client *>::iterator it = listClients.begin() ; it != listClients.end() ; ++it)
			send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
}

void	Command::names()
{
	if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
	std::string chan_name;
	if ((*_cmd)[_k].size() == 1)
	{
		for(std::vector<Channel *>::iterator it = _channels_list.begin() ; it != _channels_list.end() ; ++it)
		{
			chan_name = (*it)->getName();
			nameReply(chan_name, (*it));
		}
		return ;
	}
	Channel *finded;
	std::vector<std::string> vect_chan = splitCommas((*_cmd)[_k][1]);
	for (std::vector<std::string>::iterator it = vect_chan.begin() ; it != vect_chan.end() ; ++it)
	{
		chan_name = (*it);
		_badchan = "";
		finded = _server->findChan(chan_name);
		if (!finded)
		{
			_badchan = (*it);
			sendToClient(366);
		}
		else
		{
			_chan_name = finded;
			nameReply(chan_name, finded);
		}
	}
}

void Command::nameReply(std::string chan_name, Channel *chan)
{
	std::string msg;
	msg =":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " 353 ";
	msg += _client->getUsername() + " = " + chan_name + " :";
	std::vector<Client *> channel = chan->getClientsList();
	for (std::vector<Client *>::iterator it = channel.begin() ; it != channel.end() ; ++it)
	{
		if ((*it)->getSocket() == chan->getchanop())
			msg += "@"; 
		msg += (*it)->getNickname();
		if ((it + 1) != channel.end()) 
			msg += " ";
	}
	msg += "\r\n";
	send(_client->getSocket(), msg.c_str(), msg.size(), 0);
	sendToClient(366); //RPL_ENDOFNAMES
}

void	Command::list()
{
	if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
	if ((*_cmd)[_k].size() == 1)
	{
		for(std::vector<Channel *>::iterator it = _channels_list.begin() ; it != _channels_list.end() ; ++it)
		{
			_chan_name = *it;
			sendToClient(322);
		}
		sendToClient(323);
	}
	else if ((*_cmd)[_k].size() >= 2)
	{
		std::vector<std::string> split_chan = splitCommas((*_cmd)[_k][1]);
		for (std::vector<std::string>::iterator it = split_chan.begin() ; it != split_chan.end() ; ++it)
			listSingle(*it);
		sendToClient(323);
	}
}

void	Command::listSingle(std::string chan)
{
	Channel *finded = _server->findChan(chan);
	if (!finded) 
		return ;
	_chan_name = finded;
	sendToClient(322);
}

void	Command::invite()
{
	if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
	if ((*_cmd)[_k].size() < 3)
	{
		sendToClient(461);
		return ;
	}
	std::string target_nickname = (*_cmd)[_k][1];
	std::string chan = (*_cmd)[_k][2];
	Channel *found_chan = _server->findChan(chan);
	_badchan = chan;
	_chan_name = found_chan;
	if (!found_chan)
	{
		sendToClient(403);
		return ;
	}
	std::vector<Client *>	clients_list = found_chan->getClientsList();
	std::vector<Client *>	clients_list2 = _server->getClientList();
	for (std::vector<Client *>::iterator it = clients_list.begin() ; it != clients_list.end() ; ++it)
	{
		if ((*it)->getNickname() == _client->getNickname())
		{
			for (std::vector<Client *>::iterator it1 = clients_list.begin() ; it1 != clients_list.end() ; ++it1)
			{
				if ((*it1)->getNickname() == target_nickname)
				{
					_bad_nickname = target_nickname;
					sendToClient(443);
					return ;
				}
			}
			for (std::vector<Client *>::iterator it2 = clients_list2.begin() ; it2 != clients_list2.end() ; ++it2)
			{
				if ((*it2)->getNickname() == target_nickname)
				{
					std::string msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " INVITE " + target_nickname + " " + _chan_name->getName() + "\r\n";
					send((*it2)->getSocket(), msg.c_str(), msg.size(), 0);
					sendToClient(341);
					return ;
				}
			}
			_bad_nickname = (*_cmd)[_k][1];
			sendToClient(401);
			return ;
		}
	}
	sendToClient(442);
	return ;
}

std::vector<std::string> Command::splitCommas(std::string listchans)
{
	std::vector<std::string> vect_chan;
	std::string tmp;
	for(std::string::iterator it = listchans.begin() ; it != listchans.end() ; ++it)
	{
		if ((*it) == ',')
		{
			vect_chan.push_back(tmp);
			tmp.clear();
		}
		if ((*it) != ',')
			tmp.push_back((*it));
	}
	if (tmp.size() > 0)
		vect_chan.push_back(tmp);
	return (vect_chan);
}

void    Command::kick()
{
    if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
    if ((*_cmd)[_k].size() < 3)
    {
        sendToClient(461);
        return ;
    }
    std::string chan = (*_cmd)[_k][1];
    Channel *found_chan = _server->findChan(chan);
    _badchan = (*_cmd)[_k][1];
    _chan_name = found_chan;
    if (!found_chan)
    {
        sendToClient(403);
        return ;
    }
	std::vector<Client *> clients_list = found_chan->getClientsList();
    for (std::vector<Client *>::iterator it = clients_list.begin() ; it != clients_list.end() ; ++it)
    {
        if ((*it)->getNickname() == _client->getNickname())
        {
            for (std::vector<Client *>::iterator it1 = clients_list.begin() ; it1 != clients_list.end() ; ++it1)
            {
                if ((*it1)->getNickname() == (*_cmd)[_k][2])
                {
                    if (found_chan->getchanop() == _client->getSocket())
                    {
                        std::string msg;
                        msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " KICK " + _chan_name->getName() + " " + (*it1)->getNickname();
                        if ((*_cmd)[_k].size() > 3)
                            for (std::vector<std::string>::iterator it2 = (*_cmd)[_k].begin() ; it2 != (*_cmd)[_k].end() ; ++it2)
                                msg += " " + (*it2);
                        msg += "\r\n";
                        for (std::vector<Client *>::iterator it3 = clients_list.begin() ; it3 != clients_list.end() ; ++it3)
                            send((*it3)->getSocket(), msg.c_str(), msg.size(), 0);
                        found_chan->removeclient((*it1)->getSocket());
                        (*it1)->leaveChan(found_chan);
                        deleteEmptyChans();
                        return ;
                    }
                    sendToClient(482);
                    return ;
                }
            }
            _bad_nickname = (*_cmd)[_k][2];
            sendToClient(441);
            return ;
        }
    }
    sendToClient(442);
    return ;
}

void	Command::kill()
{
	if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
    if ((*_cmd)[_k].size() < 3)
    {
        sendToClient(461);
        return ;
    }
	if (_client->getOper() == 0)
	{
		sendToClient(481);
		return ;
	}
	if (!nickname_existence((*_cmd)[_k][1]))
	{
		_bad_nickname = (*_cmd)[_k][1];
		sendToClient(401);
		return ;
	}
	Client *killed_client = _server->findClient((*_cmd)[_k][1]);
	std::string reason;
	for (std::vector<std::string>::iterator it = (*_cmd)[_k].begin() + 2 ; it != (*_cmd)[_k].end() ; ++it)
	{
		if (it != (*_cmd)[_k].begin() + 2)
			reason += " ";
		reason += *it;
	}
	std::string	msg;
	msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername();
	msg += " KILL " + (*_cmd)[_k][1] + " :" + reason + "\r\n";
	send(killed_client->getSocket(), msg.c_str(), msg.size(), 0);
	msg = ":" + killed_client->getNickname() + "!" + killed_client->getUsername() + "@" + killed_client->getServername();
	msg += " QUIT :";
	msg += "Killed (" + _client->getNickname() + "(" + reason + "))\r\n";
	std::vector<Channel *> chan_list = (*killed_client).getClientChannelsList();
	std::vector<Client *> clients_list;
	for (std::vector<Channel *>::iterator it = chan_list.begin() ; it != chan_list.end() ; ++it)
	{
		clients_list = (*it)->getClientsList();
		for (std::vector<Client *>::iterator it1 = clients_list.begin() ; it1 != clients_list.end(); ++it1)
			send((*it1)->getSocket(), msg.c_str(), msg.size(), 0);
	}
	msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername();
	msg += " ERROR :Closing Link: " + _client->getServername();
	msg += "(Killed (" + _client->getNickname() + "(" + reason + ")))\r\n";
	send(killed_client->getSocket(), msg.c_str(), msg.size(), 0);
	for (std::vector<Channel *>::iterator it = chan_list.begin() ; it != chan_list.end() ; ++it)
	{
		if ((*it)->getchanop() == killed_client->getSocket())
			(*it)->setChanOp(0);
		(*it)->removeclient(killed_client->getSocket());
	}
	killed_client->leaveAllChan();
	deleteEmptyChans();
	killed_client->setStatus(DELETE);
}