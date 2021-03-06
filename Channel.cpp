#include "Channel.hpp"

Channel::Channel()
:	__prefix('#'),
	__name("(null"),
	__topic("(null)"),
	__size(-1)
{
	return ;
}

Channel::Channel(const Channel &rhs)
{
	*this = rhs;
}

Channel		&Channel::operator=(const Channel &rhs)
{
	this->__prefix = rhs.__prefix;
	this->__name = rhs.__name;
	this->__users = rhs.__users;
	this->__modes = rhs.__modes;
	this->__topic = rhs.__topic;
	this->__password = rhs.__password;
	this->__size = rhs.__size;
	this->__banned = rhs.__banned;
	this->__operators = rhs.__operators;
	this->__voice_privilege = rhs.__voice_privilege;
	this->__invited_list = rhs.__invited_list;
	return *this;
}

Channel::Channel(char prefix, std::string name, std::string topic)
{
	if (prefix != '#' && prefix != '+')
	{
		std::string	str = "received prefix: ";
		str += prefix;
		str += ". expected [#|+]";
	}
	this->__prefix = prefix;
	this->__name = name;
	this->__topic = topic;
	this->__size = -1;
}

Channel::Channel(std::string full_name, std::string password)
{
	this->__prefix = full_name[0];
	this->__name = full_name.substr(1);
	this->__password = password;
	this->__size = -1;
	if (password == "")
		this->__modes.set_k(true);
}

Channel::~Channel()
{
	return ;
}

bool	Channel::operator==(const Channel &rhs)
{
	return (this->__prefix == rhs.__prefix && this->__name == rhs.__name);
}

bool	Channel::operator!=(const Channel &rhs)
{
	return !(*this == rhs);
}

char	Channel::get_prefix() const
{
	return this->__prefix;
}

std::string	Channel::get_name() const
{
	return this->__name;
}

std::list<User *>	&Channel::get_users()
{
	return this->__users;
}

size_t	Channel::get_size() const
{
	return this->__size;
}

std::string		Channel::get_topic() const
{
	return this->__topic;
}

CModes	&Channel::get_modes()
{
	return this->__modes;
}

std::string	&Channel::get_password( void)
{
	return __password;
}

std::vector<User *> &Channel::get_voice_privilege( void)
{
	return (__voice_privilege);
}

void	Channel::set_topic(const std::string topic)
{
	this->__topic = topic;
}

void	Channel::set_password(std::string pass)
{
	__password = pass;
}

void	Channel::set_size(size_t size)
{
	__size = size;
}

int		Channel::add_user(User *u)
{
	this->__users.push_back(u);
	return this->__users.size();
}

int		Channel::remove_user(User *u)
{
	std::list<User *>::iterator begin;
	for (begin = this->__users.begin() ; begin != this->__users.end() && *begin != u; begin++) ;
	if (begin != this->__users.end())
		this->__users.erase(begin);
	return this->__users.size();
}

bool	Channel::has_user(User *u)
{
	std::list<User *>::iterator begin;

	for (begin = this->__users.begin() ; begin != this->__users.end(); begin++)
	{
		if (*begin == u)
			return true;
	}
	return false;
}

bool	Channel::is_user_banned(const User &user)
{
	std::list<std::string>::iterator begin;

	for (begin = this->__banned.begin() ; begin != this->__banned.end(); begin++)
	{
		if (user.get_nickname() == *begin)
			return true;
	}
	return false;
}

void	Channel::ban_user(User &user)
{
	__banned.push_back(user.get_nickname());
}

void	Channel::ban_mask(std::string str)
{
	this->__banned.push_back(str);
}

std::vector<User *> &Channel::get_operators( void)
{
	return (__operators);
}

void	Channel::add_operator(User &op)
{
	__operators.push_back(&op);
}

void	Channel::add_voice_privilege(User &op)
{
	__voice_privilege.push_back(&op);
}

void	Channel::remove_operator(User &op)
{
	for (std::vector<User *>::iterator it = this->__operators.begin(); it != this->__operators.end(); it++)
		if ((*it)->get_nickname() == op.get_nickname())
		{
			this->__operators.erase(it);
			break ;
		}
}

void	Channel::remove_voice_privilege(User &op)
{
	for (std::vector<User *>::iterator it = this->__voice_privilege.begin(); it != this->__voice_privilege.end(); it++)
		if ((*it)->get_nickname() == op.get_nickname())
		{
			this->__voice_privilege.erase(it);
			break ;
		}
}

std::string	Channel::str_name() const
{
	std::string		res = "!";
	res[0] = this->__prefix;
	res += this->__name;
	return res;
}

void	Channel::send_message(Server &serv, User &u, std::vector<std::string> message) const
{
	std::string		rep = 0;
	rep = ":" + u.get_nickname() + "!" + u.get_username() + "@" + u.get_hostname() + " " + message[0] + \
		this->__prefix + this->__name + " " + message[2] + "\r\n";
	for (std::list<User *>::const_iterator user = this->__users.begin(); user != this->__users.end(); user++)
	{
		if (u.full_id() != (*user)->full_id())
		{
			std::cerr << (*user)->get_nickname() << " > " + rep;
			serv.write_socket((*user)->get_fd(), rep);
		}
	}
}

void	Channel::send(Server &serv, std::string message)
{
	for (std::list<User *>::const_iterator user = this->__users.begin(); user != this->__users.end(); user++)
	{
		std::cerr << (*user)->get_nickname() << " > " + message;
		serv.write_socket((*user)->get_fd(), message);
	}
}

bool	Channel::is_operator(std::string nick)
{
	for(std::vector<User *>::iterator it = this->__operators.begin(); it != this->__operators.end(); it++)
		if ((*it)->get_nickname() == nick)
			return true;
	return (false);
}

bool	Channel::has_voice(std::string nick)
{
	for(std::vector<User *>::iterator it = this->__voice_privilege.begin(); it != this->__voice_privilege.end(); it++)
		if ((*it)->get_nickname() == nick)
			return true;
	return (false);
}
