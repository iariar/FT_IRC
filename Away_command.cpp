#include "Server.hpp"
#include "msg_parse.hpp"


int		Server::AWAY_handler(msg_parse &command, User &user)
{
	if (command.get_additional_param().size())
	{
		user.set_modes(1);
		if (!command.get_additional_param().empty())
		{
			user.set_away_msg(command.get_additional_param());
			std::string	full_msg = ":" + this->__name + " 306 AWAY " +  command.get_additional_param() + "\n"/* + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname() + "\n"*/; 
			send(user.get_fd(), full_msg.c_str(), full_msg.size(), 0);
		}
		else
			write_reply(user, RPL_NOWAWAY, command);
	}
	else
	{
		user.unset_modes(1);
		user.get_away_msg().clear();
		write_reply(user, RPL_UNAWAY, command);
	}
	return (1);
}

void	Server::remove_user(User &user)
{
	for (std::list<User>::iterator it = __users.begin(); it != __users.end(); it++)
	{
		if (*it == user)
			__users.erase(it);
	}
}

void	Server::QUIT_handler(User &user)
{
	std::string full_msg = "Closing Link: HOST_NAME";

	// if (command.get_cmd_params().size())
	// 	full_msg = ":" + this->__name + "@" + command.get_cmd_params().front() + " 433 :Nickname is already in use\n" + user.get_nickname() + "!" + user.get_username() + "@" + user.get_hostname();
	if (user.get_channels().size())
		part_from_all_channels(user);
	write_socket(user.get_fd(), full_msg);
	this->disconnect_user(user);
	remove_user(user);
	__list_nicks.erase(user.get_nickname());
}
