#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <sstream>
#include <map>
#include <time.h>

class Server;
class Client;
class Channel;

class Command
{
	public:
		Command(Client *client, Server *irc);
		~Command();

	private:
		void    nick();
		void    pass();
		void    user();
		void	cap();
		void    oper();
		void    privmsg();
		void	notice();
		void	quit();
		void 	kick();
		void 	topic();
		void 	part();
		void 	join();
		void	invite();
		void 	list();
		void	names();
		void	ping();
		void	kill();


		std::vector<std::string> splitCommas(std::string listchans);
		void	listSingle(std::string chan);
		void	joinSingle(std::string current_chan);
		void	quitChan(std::string chan);
		void	deleteEmptyChans();
		void 	nameReply(std::string chan_name, Channel *chan);

		void	fatal_error(std::string error);

		bool    valid_cmd(std::string cmd);
		void    error_cmd(std::string s);

		std::string	getTime();
		std::string	insert_zeros(int n);
		template <typename T>
		std::string	to_string(T n);

		int        check_nickname(std::string nickname);
		int        nickname_existence(std::string nickname);

		void        sendToClient(int n);
		void		sendToTarget(std::string target, int target_fd, bool notice);
		void		sendToChannel(Channel *channel, bool notice);

		std::vector<std::vector<std::string> >      *_cmd;
		std::vector<std::string>        _cmd_list;
		std::map<std::string, void(Command::*)()> _launch_cmd;
		int         _k;

        std::string _pass;

		std::string _oper_name;
		std::string _oper_pass;
		std::string	_creation_time;
		Channel	*_chan_name;
		std::string _badchan;
		std::string	_bad_nickname;

		Client      *_client;
		std::vector<Client *>    _clients_list;
		std::vector<Channel *>    _channels_list;
		Server		*_server;
};


#endif
