#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "core.hpp"
# include "Buffer.hpp"

enum status_e { READING_HEADERS, READING_BODY,
				READY_TO_WRITE, WRITING, CLOSING };

class Connection
{
	public:
		Connection( void );
		~Connection( void );

	private:
		int			code;
		int			status;
		int			conn_sock;
		Buffer		sending;
		Buffer		receving;
};

#endif
