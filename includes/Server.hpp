#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <iterator>

namespace cfg
{
	typedef struct						s_location
	{
		std::string								_location;
		std::vector<std::string>				_allow;
		std::pair<int, std::string>				_return;
		std::string								_root;
		bool									_autoindex;
		std::vector<std::string>				_index;
		std::map<std::string, std::string>		_cgi_pass;
		std::string								_upload_store;
	}									t_location;

	class Server
	{
		public:

			int																	_socket;
			size_t																_listen;
			size_t																_client_max_body_size;
			std::vector<std::string>											_server_name;
			std::map<int, std::string>								_error_page;
			std::vector<t_location>												_locations;

			//	Constructor and destructor
			Server() : _client_max_body_size(0) {};
			~Server() {};
	};
}

# endif