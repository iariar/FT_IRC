#include "msg_parse.hpp"
#include "Server.hpp"

void		Server::JOIN_handler(User &user, msg_parse &command)
{
	std::string channels = command.get_cmd_params()[0];
	if (channels == "0")
	{
		if (user.get_nb_channels())
		{
			//part
			std::cout << "User should leave all channels" << std::endl;
		}
	}
	if (command.get_cmd_params().size() == 2)
	{
		std::string keys = command.get_cmd_params()[1];
		size_t channel_index = 0;
		// int prev_channel_index = 0;
		size_t key_index = 0;
		int prev_key_index = 0;
		int prev_chan_index = 0;
		std::string channel_name;
		std::string key;
		while (((key_index = keys.find(',', key_index)) != std::string::npos || prev_key_index < keys.length()) && ((channel_index = channels.find(',', channel_index)) != std::string::npos || prev_chan_index < channels.length()))
		{
			channel_index == std::string::npos ? channel_index = channels.length() : 0;
			key_index == std::string::npos ? key_index = keys.length() : 0;
			channel_name = channels.substr(prev_chan_index, channel_index - prev_chan_index);
			key = keys.substr(prev_key_index, key_index - prev_key_index);
			if (find_channel(channel_name[0],channel_name.substr(1, channel_name.length() - 1)) == __channels.end())
			{
				add_channel(channel_name[0],channel_name.substr(1, channel_name.length() - 1), key);
				Channel chan = *find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1));
				chan.add_operator(user.get_nickname());
				chan.set_password(key);
			}
			else
			{
				Channel cho = *find_channel(channel_name[0], channel_name.substr(1, channel_name.length() - 1));
				if (find_user_in_channel(user, cho) == *cho.get_users().end())
				{
					if (cho.get_password() == key)
					{
						if (cho.is_user_banned(user))
							write_reply(user, ERR_BANNEDFROMCHAN, command);
						else if (cho.get_modes().get_i())
							write_reply(user, ERR_INVITEONLYCHAN, command);
						else
						{
							
							write_reply(user, RPL_TOPIC, command);
							for (std::list<User>::iterator it = cho.get_users().begin(); it != cho.get_users().end(); it++)
							{
								std::string full_msg = user.full_id() + " JOIN " + channel_name + "\n";
								write_socket((*it).get_fd() , full_msg);
							}
							cho.add_user(&user);
							// write_reply(user, RPL_NAMREPLY, command);
							// write_reply(user, RPL_ENDOFNAMES, command);
						}
					}
					else
					{
						write_reply(user, ERR_BADCHANNELKEY, command);
					}
				}
			}
			key_index++;
			prev_key_index = key_index;
			channel_index++;
			prev_chan_index = channel_index;
		}
	}
	else if (command.get_cmd_params().size() > 2)
		write_socket(user.get_fd(), "more args than necessary\n");
	else
		write_reply(user, ERR_NEEDMOREPARAMS, command);
	// std::cout << "channel names list" << std::endl;
	// for (std::list<Channel>::iterator it = get_channels().begin(); it != get_channels().end(); it++)
	// {
	// 	std::cout << "|" << (*it).get_prefix() << (*it).get_name() << "| pass = |" << (*it).get_password() << std::endl;
	// }
}