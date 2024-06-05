#include "Server.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "Client.hpp"

template <typename T>
std::string	Command::to_string(T n)
{
    std::ostringstream  s;
    s << n;
    return (s.str());
}

std::string Command::insert_zeros(int n)
{
    std::string s("");
    if (n >=0 && n <= 9)
    {
        s = to_string("00");
        return (s);
    }
    else if (n >= 10 && n <= 99)
    {
        s = to_string("0");
        return (s);
    }
    return (s);
}

void    Command::sendToClient(int n)
{
    std::string msg;

    if (n >= 1 && n <= 5)
        msg = ":" + _client->getServername() + " " + insert_zeros(n) + to_string(n) + " " + _client->getNickname() + " :";
    else
        msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _client->getServername() + " " + insert_zeros(n) + to_string(n) + " ";

    switch (n)
    {
        case 1:
        {
            msg += "Welcome to the Internet Relay Network " + _client->getNickname() + "\r\n";
            break;
        }
        case 2:
        {
            msg += "Your host is " + _client->getServername() + ", running on version [irc]\r\n";
            break;
        }
        case 3:
        {
            msg += "This server was created at: " + _creation_time + " \r\n";
            break;
        }
        case 4:
        {
            msg += _client->getServername() + " version [irc]. Available user MODE : +o . Available channel MODE : none. \r\n";
            break;
        }
		case 322: //RPL_LIST
		{
			msg += _client->getUsername() + " " + _chan_name->getName() + " " + to_string(_chan_name->getnbclt()) + " :" + _chan_name->getTopic() + "\r\n";
			break ;
		}
		case 323: //RPL_LISTEND
		{
			msg += _client->getUsername() + " :End of /LIST\r\n";
			break ;
		}
		case 331: //RPL_NOTOPIC
		{
			msg += _client->getUsername() + " " + _chan_name->getName() + " :No topic is set\r\n";
			break;
		}
		case 332: //RPL_TOPIC
		{
			msg += _client->getUsername() + " " + _chan_name->getName() + " :" + _chan_name->getTopic() + "\r\n";
			break;
		}
		case 333: //RPL_TOPICWHOTIME
		{
			msg += _client->getUsername() + " " + _chan_name->getName() + " " + _client->getNickname() + " " + "notime" + "\r\n";
			break;
		}
		case 341: //RPL_INVITING
		{
			msg += _client->getUsername() + " " + (*_cmd)[_k][1] + " " + _chan_name->getName() + "\r\n";
			break;
		}
		case 366: //RPL_ENDOFNAMES
		{
			if (_badchan != "")
				msg += _client->getUsername() + " " + _badchan + " :End of /NAMES list\r\n";	
			else
				msg += _client->getUsername() + " " + _chan_name->getName() + " :End of /NAMES list\r\n";	
			break ;
		}
		case 381: //RPL_YOUREOPER
		{
			msg += _client->getUsername() + " :You are now an IRC operator\r\n";
			break;
		}
		case 401: //ERR_NOSUCHNICK
		{
			msg += _client->getUsername() + " " + _client->getNickname() + " :No such nick\r\n";
			break;
		}
		case 403: //ERR_NOSUCHCHANNEL
		{
			msg += _client->getUsername() + " " + _badchan + " :No such channel\r\n";
			break;
		}
		case 404: //ERR_CANNOTSENDTOCHAN
		{
			msg += _client->getUsername() + " " + (*_cmd)[_k][1] + " :Cannot send to channel\r\n";
			break;
		}
		case 412: //ERR_NOTEXTTOSEND
		{
			msg += ":No text to send\r\n";
			break;
		}
		case 421: //ERR_UNKNOWNCOMMAND
		{
			msg += _client->getUsername() + " " + (*_cmd)[_k][0] + " :Unknown command\r\n";
			break;
		}
		case 431: //ERR_NONICKNAMEGIVEN
		{
			msg += " :No nickname given\r\n";	
			break;
		}
		case 432: //ERR_ERRONEUSNICKNAME
		{
			msg += _client->getUsername() + " " + _client->getNickname() + " :Erroneus nickname\r\n";	
			break;
		}
		case 433: //ERR_NICKNAMEINUSE
		{
			msg += _client->getUsername() + " " +  _client->getNickname() + " :Nickname is already in use\r\n";	
			break;
		}
		case 441: //ERR_USERNOTINCHANNEL
		{
			msg += _client->getUsername() + " " + _chan_name->getName() + " " + _bad_nickname + " :is not on that channel\r\n";
			break;
		}
		case 442: //ERR_NOTONCHANNEL
		{
			msg += _client->getUsername() + " " + _chan_name->getName() + " :You're not on that channel\r\n";
			break;
		}
		case 443: //ERR_USERONCHANNEL
		{
			msg += _client->getUsername() + " " + _chan_name->getName() + " " + _bad_nickname + " :is already on channel\r\n";
			break;
		}
		case 451: //ERR_NOTREGISTERED
		{
			msg += _client->getUsername() +  " :You have not registered\r\n";
			break;
		}
		case 461: //ERR_NEEDMOREPARAMS
		{
			msg += _client->getUsername() + " " + (*_cmd)[_k][0] + " :Not enough parameters\r\n";
			break;
		}
		case 462: //ERR_ALREADYREGISTERED
		{
			msg += _client->getUsername() + " :You may not reregister\r\n";	
			break;
		}
		case 464: //ERR_PASSWDMISMATCH
		{
			msg += _client->getUsername() + " :Password incorrect\r\n";	
			break;
		}
		case 481: //ERR_NOPRIVILEGES
		{
			msg += _client->getUsername() + " :Permission Denied- You're not an IRC operator\r\n";
			break;
		}
		case 482: //ERR_CHANOPRIVSNEEDED
		{
			msg += _client->getUsername() + " " +  _chan_name->getName() + " :You're not channel operator \r\n";	
			break;
		}
		case 491: //ERR_NOOPERHOST
		{
			msg += _client->getUsername() + " :No O-lines for your host\r\n";	
			break;
		}
		default :
		{
			msg = "DEFAULT CASE\r\n";
			break ;
		}
    }
	//std::cout << msg << std::endl;
    send(_client->getSocket(), msg.c_str(), msg.size(), 0);
}