#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <iterator>

namespace scfg
{
	class Server
	{
	public :

		typedef std::map< std::string, std::vector< std::string> >	t_location;

	private:

		std::string															_server_name;
		int																	_listen;
		t_location															_location;

	public:

		//	Constructor and destructor
		Server();
		~Server();

		//	Getters
		std::string												getServerName(void);
		int														getListen(void);
		location												getLocation(void);

		//	Setters
		void													setServerName(std::string server_name);
		void													setListen(int listen);
		void													setLocation(location location);
	};
}

# endif