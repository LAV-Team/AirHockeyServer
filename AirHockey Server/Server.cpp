#include <iostream>
#include "Transceiver.hpp"

class Server
	: public boost::enable_shared_from_this<Server>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr<Server> SharedPtr;

	static SharedPtr Create(unsigned short port = 4444)
	{
		SharedPtr server{ new Server{ port } };
		return server;
	}

	void Start()
	{
		if (isStarted_) {
			return;
		}
		isStarted_ = true;
		CreateTransceiver();
		service_.run();
	}

	void Stop()
	{
		if (!isStarted_) {
			return;
		}
		isStarted_ = false;
		acceptor_.close();
	}

	void CreateTransceiver()
	{
		Transceiver::SharedPtr client = Transceiver::Create(service_);
		client->SetErrorHandler(BIND_WITH_2_ARGS(ErrorHandler, client, _1));
		client->SetAnswerHandler(BIND_WITH_2_ARGS(AnswerHandler, client, _1));
		acceptor_.async_accept(client->Sock(), BIND_WITH_2_ARGS(HandleAccept, client, _1));
	}

	void HandleAccept(Transceiver::SharedPtr client, boost::system::error_code const& error)
	{
		client->StartReading();
		CreateTransceiver();
	}

	void AnswerHandler(Transceiver::SharedPtr client, std::string const& answer)
	{
		std::cout << answer << std::endl;
		client->Send(answer);
	}

	void ErrorHandler(Transceiver::SharedPtr client, boost::system::error_code const& error)
	{
		std::cout << error.message() << std::endl;
	}

private:
	typedef Server SelfType;

	boost::asio::io_service service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	bool isStarted_;

	Server(unsigned short port)
		: service_{}
		, acceptor_{ service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port) }
		, isStarted_{ false }
	{}
};

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	Server::SharedPtr server{ Server::Create(4444) };
	server->Start();
}