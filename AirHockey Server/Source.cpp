#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#define BIND(x) boost::bind(&SelfType::x, shared_from_this())
#define BIND_WITH_1_ARG(x, y) boost::bind(&SelfType::x, shared_from_this(), y)
#define BIND_WITH_2_ARGS(x, y, z) boost::bind(&SelfType::x, shared_from_this(), y, z)

static size_t const MAX_MESSAGE_LENGTH = 64U;
static char const MESSAGE_END = '\n';

class ClientHandler
	: public boost::enable_shared_from_this<ClientHandler>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr <ClientHandler> SharedPtr;

	static SharedPtr Create(boost::asio::io_service& service)
	{
		SharedPtr clientHandler{ new ClientHandler{ service } };
		return clientHandler;
	}

	boost::asio::ip::tcp::socket& Sock()
	{
		return sock_;
	}

	bool IsStarted()
	{
		return isStarted_;
	}

	void Start()
	{
		if (isStarted_) {
			return;
		}
		isStarted_ = true;
		Read();
	}

	void Stop()
	{
		if (!isStarted_) {
			return;
		}
		isStarted_ = false;
		sock_.close();
	}

	void Read()
	{
		boost::asio::async_read(
			sock_,
			boost::asio::buffer(readBuffer_),
			BIND_WITH_2_ARGS(IsReadingCompleted, _1, _2),
			BIND_WITH_2_ARGS(OnRead, _1, _2)
		);
	}

	void Write(std::string const& message)
	{
		if (!isStarted_) {
			return;
		}
		std::copy(message.begin(), message.end(), writeBuffer_);

		sock_.async_write_some(
			boost::asio::buffer(writeBuffer_, message.size()),
			BIND_WITH_2_ARGS(OnWrite, _1, _2)
		);
	}

	size_t IsReadingCompleted(boost::system::error_code const& error, size_t bytes)
	{
		if (error) {
			return 0;
		}
		// If > max length => read last part
		// 0 if comleted else 1
		return std::find(readBuffer_, readBuffer_ + bytes, MESSAGE_END) < readBuffer_ + bytes ? 0 : 1;
	}

	void OnRead(boost::system::error_code const& error, size_t bytes)
	{
		if (!error) {
			std::string message(readBuffer_, bytes);
			std::cout << message;
			Write(message);
		}
		Stop();
	}

	void OnWrite(boost::system::error_code const& error, size_t bytes)
	{
		Read();
	}

private:
	typedef ClientHandler SelfType;

	boost::asio::ip::tcp::socket sock_;
	char readBuffer_[MAX_MESSAGE_LENGTH];
	char writeBuffer_[MAX_MESSAGE_LENGTH];
	bool isStarted_;

	ClientHandler(boost::asio::io_service& service)
		: sock_{ service }
		, isStarted_{ false }
	{}
};

class Server
	: public boost::enable_shared_from_this<Server>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr <Server> SharedPtr;

	static SharedPtr Create(unsigned short port = 4444)
	{
		SharedPtr server{ new Server{ port } };
		return server;
	}

	bool IsStarted()
	{
		return isStarted_;
	}

	void Start()
	{
		if (isStarted_) {
			return;
		}
		isStarted_ = true;

		ClientHandler::SharedPtr client = ClientHandler::Create(service_);
		acceptor_.async_accept(client->Sock(), BIND_WITH_2_ARGS(HandleAccept, client, _1));

		service_.run();
	}

	void Stop() 
	{
		if (!isStarted_) {
			return;
		}
		isStarted_ = false;

		service_.stop();
	}

	void HandleAccept(ClientHandler::SharedPtr client, boost::system::error_code const& error)
	{
		client->Start();
		
		ClientHandler::SharedPtr newClient = ClientHandler::Create(service_);
		acceptor_.async_accept(newClient->Sock(), BIND_WITH_2_ARGS(HandleAccept, newClient, _1));
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
	Server::SharedPtr server{ Server::Create(8001) };
	server->Start();
}