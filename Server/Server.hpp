#ifndef __SERVER_INCLUDED__
#define __SERVER_INCLUDED__

#include <iostream>
#include <unordered_map>
#include "Client.hpp"

namespace HockeyNet
{
	class Server;
	typedef boost::shared_ptr<Server> ServerPtr;

	class Server
		: public boost::enable_shared_from_this<Server>
		, boost::noncopyable
	{
	public:
		static ServerPtr Create(unsigned short port);

		void Start();
		void Stop();

	private:
		typedef Server SelfType;

		bool isStarted_;
		boost::asio::io_service service_;
		std::thread serviceThread_;
		boost::asio::ip::tcp::acceptor acceptor_;

		std::list<ClientPtr> freeClients_;
		std::list<ClientPtr> waitingClients_;
		std::list<ClientPtr> busyClients_;

		Server(unsigned short port);

		std::string GenerateSessionId_();
		void MakeSessions_();
		void CreateTransceiver_();

		void AcceptHandler_(ClientPtr client, boost::system::error_code const& error);
		void ErrorHandler_(ClientPtr client, boost::system::error_code const& error);
		void AnswerHandler_(ClientPtr client, std::string const& answer);
		void CloseHandler_(ClientPtr client);
	};
};

#endif // __SERVER_INCLUDED__
