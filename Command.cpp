#include "Command.hpp"
#include "Server.hpp"

Command::Command(Client *client, Server *irc) : _k(0), _pass(irc->getPass()), _oper_name(OPER), _oper_pass(OPERPWD), _creation_time(getTime())
{
    _client = client;
    _cmd = client->getCommand();
    _clients_list = irc->getClientList();
    _channels_list = irc->getChanList();
    _server = irc;

    _cmd_list.push_back("PASS");
    _cmd_list.push_back("NICK");
    _cmd_list.push_back("USER");
    _cmd_list.push_back("OPER");
    _cmd_list.push_back("INVITE");
    _cmd_list.push_back("PART");
    _cmd_list.push_back("TOPIC");
    _cmd_list.push_back("JOIN");
    _cmd_list.push_back("PRIVMSG");
    _cmd_list.push_back("QUIT");
    _cmd_list.push_back("NOTICE");
    _cmd_list.push_back("KICK");
    _cmd_list.push_back("KILL");
    _cmd_list.push_back("NAMES");
    _cmd_list.push_back("kill");
    _cmd_list.push_back("CAP");
    _cmd_list.push_back("LIST");
    _cmd_list.push_back("PING");

    _launch_cmd["PASS"] = &Command::pass;
    _launch_cmd["CAP"] = &Command::cap;
    _launch_cmd["NICK"] = &Command::nick;
    _launch_cmd["USER"] = &Command::user;
    _launch_cmd["OPER"] = &Command::oper;
    _launch_cmd["PRIVMSG"] = &Command::privmsg;
    _launch_cmd["NOTICE"] = &Command::notice;
    _launch_cmd["QUIT"] = &Command::quit;
    _launch_cmd["INVITE"] = &Command::invite;
    _launch_cmd["KICK"] = &Command::kick;
    _launch_cmd["TOPIC"] = &Command::topic;
    _launch_cmd["PART"] = &Command::part;
    _launch_cmd["JOIN"] = &Command::join;
    _launch_cmd["LIST"] = &Command::list;
    _launch_cmd["NAMES"] = &Command::names;
    _launch_cmd["PING"] = &Command::ping;
    _launch_cmd["KILL"] = &Command::kill;
    _launch_cmd["kill"] = &Command::kill;


    for (std::vector<std::vector<std::string> >::iterator it =_cmd->begin() ; it != _cmd->end() ; ++it)
    {
        if ((it->empty()))
        	return ;
        for (std::vector<std::string>::iterator it1 = _cmd_list.begin() ; it1 != _cmd_list.end() ; ++it1)
        {
            if ((*it)[0] == *it1)
            {
                (this->*_launch_cmd[(*it)[0]])();
            }
        }
        if (!valid_cmd((*it)[0]))
        {
            error_cmd((*it)[0]);
        }
        ++_k;
    }
}

Command::~Command()
{
    _client->clear_command();
}

std::string Command::getTime()
{
    time_t  current_time = time(0);
    struct tm   time_struct;
    char    buffer[128];
    time_struct = *localtime(&current_time);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d.%X", &time_struct);
    std::string time(buffer);
    return (time);
}

void    Command::fatal_error(std::string error)
{
    std::string msg;
    msg = ":" + _client->getNickname() + " " + "ERROR" + " " + error + "\r\n";
    send(_client->getSocket(), msg.c_str(), msg.size(), 0);
    _client->setStatus(DELETE);
}

bool    Command::valid_cmd(std::string cmd)
{
    for (std::vector<std::string>::iterator it = _cmd_list.begin() ; it != _cmd_list.end() ; ++it)
    {
        if ((*it) == cmd)
        {
            return(1);
        }
    }
    return (0);
}

void    Command::error_cmd(std::string s)
{
    std::cout << "You have entered a wrong command : " << s << std::endl;
    sendToClient(421);
}


void    Command::cap()
{

}

void    Command::pass()
{
    if (_client->getStatus() != TO_REGISTER)
    {
        sendToClient(462);
        return ;
    }
    else if ((*_cmd)[_k].size() < 2)
    {
        sendToClient(461);
        return ;
    }
    else if ((*_cmd)[_k].size() > 2)
    {
        return ;
    }
    if ((*_cmd)[_k][1] != _pass)
    {
        sendToClient(464);
        fatal_error("Error : incorrect password");
        return ;
    }
    else
    {
        _client->setStatus(PASSWORD);
    }
}


int    Command::check_nickname(std::string nickname)
{
    std::string check(" ,?!.@*");
    if (nickname[0] == ':' || nickname[0] == '$')
        return (1);
    for (std::string::iterator it = nickname.begin() ; it != nickname.end() ; it++)
    {
        if (check.find((*it)) != std::string::npos)
            return (1);
    }
    return (0);
}

int        Command::nickname_existence(std::string nickname)
{
    for (std::vector<Client *>::iterator it = _clients_list.begin() ; it != _clients_list.end() ; ++it)
    {
        if ((*it)->getNickname() == nickname)
            return (1);
    }
    return (0);
}

void    Command::nick()
{
    if (_client->getStatus() == TO_REGISTER || _client->getStatus() == DELETE)
    {
        sendToClient(464);
        fatal_error("You have to enter the right password before NICK and USER1");
        return ;
    }
    if ((*_cmd)[_k].size() == 1)
    {
        sendToClient(431);
        return ;
    }
    else if (check_nickname((*_cmd)[_k][1]) || (*_cmd)[_k].size() > 2)
    {
        sendToClient(432);
        return ;
    }
    else if (nickname_existence((*_cmd)[_k][1]))
    {
        sendToClient(433);
        if(_client->getStatus() == PASSWORD)
            fatal_error("Nickname already in use");
        return ;
    }
    else
    {
        std::string msg;
        msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " NICK " + (*_cmd)[_k][1] + "\r\n";
        for (std::vector<Client *>::iterator it = _clients_list.begin() ; it != _clients_list.end() ; ++it)
        {
            send((*it)->getSocket(), msg.c_str(), msg.size(), 0);
        }
        _client->setNickname((*_cmd)[_k][1]);
    }
}

void    Command::user()
{
    if (_client->getStatus() == TO_REGISTER || _client->getStatus() == DELETE)
    {
        sendToClient(464);
        fatal_error("You have to enter the right password before NICK and USER2");
        return ;
    }
    else if (_client->getStatus() == REGISTERED)
    {
        sendToClient(462);
        return ;
    }
    else if ((*_cmd)[_k].size() < 5)
    {
        sendToClient(461);
        return ;
    }
    else if ((*_cmd)[_k][4][0] != ':')
    {
        return ;
    }
    else
    {
        _client->setRealname((*_cmd)[_k][4]);
        _client->setUsername((*_cmd)[_k][1]);
        _client->setServername((*_cmd)[_k][3]);
    }
    _client->setStatus(REGISTERED);
    sendToClient(1);
    sendToClient(2);
    sendToClient(3);
    sendToClient(4);
}

void    Command::oper()
{
    if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
    if ((*_cmd)[_k].size() != 3)
    {
        sendToClient(461);
        return ;
    }
    else if ((*_cmd)[_k][1] == _oper_name && (*_cmd)[_k][2] == _oper_pass)
    {
        _client->setOper(1);
        sendToClient(381);
        return ;
    }
    else
    {
        sendToClient(464);
        return ;
    }
}

void		Command::sendToTarget(std::string target, int target_fd, bool notice)
{
    std::string msg;
    if (notice)
        msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " NOTICE " + target + " ";
    else
        msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " PRIVMSG " + target + " ";
    for (std::vector<std::string>::iterator it = (*_cmd)[_k].begin() + 2 ; it != (*_cmd)[_k].end() ; ++it)
    {
        if (it != (*_cmd)[_k].begin() + 2)
            msg += " ";
        msg += *it;
    }
    msg += "\r\n";
    send(target_fd, msg.c_str(), msg.size(), 0);
}

void    Command::sendToChannel(Channel *channel, bool notice)
{
    std::string msg;
    if (notice)
        msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@localhost NOTICE " + (*channel).getName() + " ";
    else
        msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@localhost PRIVMSG " + (*channel).getName() + " ";
    for (std::vector<std::string>::iterator it = (*_cmd)[_k].begin() + 2 ; it != (*_cmd)[_k].end() ; ++it)
    {
        if (it != (*_cmd)[_k].begin() + 2)
            msg += " ";
        msg += *it;
    }
    msg += "\r\n";
    std::vector<Client *> client = channel->getClientsList();
    for (std::vector<Client *>::iterator it1 = client.begin() ; it1 != client.end() ; ++it1)
    {
        if ((*it1) != _client)
        {
            send((*it1)->getSocket(), msg.c_str(), msg.size(), 0);
        }
    }
}

void    Command::privmsg()
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
    if ((*_cmd)[_k].size() == 2)
    {
        sendToClient(412);
        return ;
    }
    std::string target = (*_cmd)[_k][1];
    if (target[0] == '#')
    {
        for (std::vector<Channel *>::iterator it = _channels_list.begin() ; it != _channels_list.end() ; ++it)
        {
            if ((*it)->getName() == target)
            {
                std::vector<Client *> vec = (*it)->getClientsList();
                for(std::vector<Client *>::iterator it1 = vec.begin() ; it1 != vec.end(); ++it1)
                {
                    if ((*it1)->getNickname() == _client->getNickname())
                    {
                        sendToChannel((*it), 0);
                        return ;
                    }
                }
            }
        }
        sendToClient(404);
        return ;
    }
    else
    {
        if (!nickname_existence(target))
        {
            sendToClient(401);
            return ;
        }
        for (std::vector<Client *>::iterator it = _clients_list.begin() ; it != _clients_list.end() ; ++it)
        {
            if ((*it)->getNickname() == target)
            {
                sendToTarget(target, (*it)->getSocket(), 0);
                return ;
            }
        }
    }
    sendToClient(401);
}

void    Command::notice()
{
    if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
    if ((*_cmd)[_k].size() < 3)
        return ;
    std::string target = (*_cmd)[_k][1];
    Client *client;
    if (target[0] == '#')
    {
        for (std::vector<Channel *>::iterator it = _channels_list.begin() ; it != _channels_list.end() ; ++it)
        {
            if ((*it)->getName() == target)
            {
                client = (*it)->findClt(_client->getNickname());
                if (!client)
                    return ;
                sendToChannel((*it), 1);
                return ;
            }
        }
    }
    else
    {
        for (std::vector<Client *>::iterator it = _clients_list.begin() ; it != _clients_list.end() ; ++it)
        {
            if ((*it)->getNickname() == target)
            {
                sendToTarget(target, (*it)->getSocket(), 1);
                return ;
            }
        }
    }
}

void    Command::quit()
{
    if (_client->getStatus() == TO_REGISTER || _client->getStatus() == PASSWORD)
    {
        sendToClient(451);
        return ;
    }
    std::string msg;
    std::string reason;
    msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " QUIT :";
    if ((*_cmd)[_k].size() == 1)
        reason = "Quit:";
    else
        reason = "Quit";
    for (std::vector<std::string>::iterator it = (*_cmd)[_k].begin() + 1 ; it != (*_cmd)[_k].end() ; ++it)
    {
        if (it != (*_cmd)[_k].begin() + 1)
            reason += " ";
        if (it == (*_cmd)[_k].begin() + 1)
        {
            if ((*it)[0] == ':')
                (*it).insert(1, " ");
            else
            {
                (*it).insert(0, ":");
                (*it).insert(1, " ");
            }
        }
        reason += *it;
    }
    msg += "\r\n";
    std::vector<Client *> clt;
    std::vector<Channel *> chan = _client->getClientChannelsList();
    for (std::vector<Channel *>::iterator it = chan.begin() ; it != chan.end() ; ++it)
    {
        clt = (*it)->getClientsList();
        for (std::vector<Client *>::iterator it2 = clt.begin() ; it2 != clt.end() ; ++it2)
        {
            if ((*it2) != _client)
                send((*it2)->getSocket(), msg.c_str(), msg.size(), 0);
        }
    }
    _client->remove_client_from_channel(_client);
    _client->leaveAllChan();
    fatal_error(msg);
}

void    Command::deleteEmptyChans()
{
    for (std::vector<Channel *>::iterator it = _channels_list.begin() ; it != _channels_list.end() ; ++it)
    {
        if ((*it)->getClientsList().size() == 0)
        {
            _server->DelChan((*it)->getName());
        }   
    }
}

void    Command::ping()
{
    if((*_cmd)[_k][1].empty())
        sendToClient(0);
    else
    {
        std::string msg;
        msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " ";
        msg += "PONG :" + _client->getNickname() + "\r\n";
        send(_client->getSocket(), msg.c_str(), msg.size(), 0);
    }
}
