#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#define BIND(a) boost::bind(&SelfType::a, shared_from_this())
#define BIND_WITH_1_ARG(a, b) boost::bind(&SelfType::a, shared_from_this(), b)
#define BIND_WITH_2_ARGS(a, b, c) boost::bind(&SelfType::a, shared_from_this(), b, c)
#define BIND_WITH_3_ARGS(a, b, c, d) boost::bind(&SelfType::a, shared_from_this(), b, c, d)

static size_t const BUFFER_LENGTH = 256U;
static char const COMMAND_END = ';';
static char const MESSAGE_END = '\n';



class Receiver
	: public boost::enable_shared_from_this<Receiver>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr <Receiver> SharedPtr;
	typedef boost::function<void(std::string const&)> AnswerHandler;
	typedef boost::function<void(boost::system::error_code const&)> ErrorHandler;

	static SharedPtr Create(boost::asio::io_service& service, AnswerHandler commandHandler, ErrorHandler errorHandler)
	{
		SharedPtr clientHandler{ new Receiver{ service, commandHandler, errorHandler } };
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
		
		message_.clear();
		Read_();
	}

	void Stop()
	{
		if (!isStarted_) {
			return;
		}
		isStarted_ = false;

		sock_.close();
	}

private:
	typedef Receiver SelfType;

	boost::asio::ip::tcp::socket sock_;
	bool isStarted_;
	char readBuffer_[BUFFER_LENGTH];
	std::string message_;
	AnswerHandler answerHandler_;
	ErrorHandler errorHandler_;

	Receiver(boost::asio::io_service& service, AnswerHandler answerHandler, ErrorHandler errorHandler)
		: sock_{ service }
		, isStarted_{ false }
		, answerHandler_{ answerHandler }
		, errorHandler_{ errorHandler }
	{}

	void Read_()
	{
		if (!isStarted_) {
			return;
		}

		boost::asio::async_read(
			sock_,
			boost::asio::buffer(readBuffer_),
			BIND_WITH_2_ARGS(IsReadingCompleted_, _1, _2),
			BIND_WITH_2_ARGS(OnRead_, _1, _2)
		);
	}

	size_t IsReadingCompleted_(boost::system::error_code const& error, size_t bytes)
	{
		return error || (bytes > 0 && (
			readBuffer_[bytes - 1] == COMMAND_END || readBuffer_[bytes - 1] == MESSAGE_END
		)) ? 0 : 1;
	}

	void OnRead_(boost::system::error_code const& error, size_t bytes)
	{
		if (error) {
			Stop();
			errorHandler_(error);
			return;
		}

		message_ += std::string{readBuffer_, bytes};

		if (bool isEnd{ message_.back() == MESSAGE_END }; message_.back() == COMMAND_END || isEnd) {
			message_.pop_back();
			if (!message_.empty()) {
				answerHandler_(message_);
			}
			if (isEnd) {
				Stop();
				return;
			}
			else {
				message_.clear();
				Read_();
			}
		}
		else {
			Read_();
		}
	}
};



class Transmitter
	: public boost::enable_shared_from_this<Transmitter>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr<Transmitter> SharedPtr;
	typedef boost::function<void(boost::system::error_code const&)> ErrorHandler;

	static SharedPtr Create(boost::asio::io_service& service,
		boost::asio::ip::tcp::endpoint const& ep, ErrorHandler errorHandler)
	{
		SharedPtr sender{ new Transmitter{ service, ep, errorHandler } };
		return sender;
	}

	boost::asio::ip::tcp::socket& Sock()
	{
		return sock_;
	}

	void Send(std::string const& message)
	{
		if (sock_.is_open()) {
			Write_(message);
		}
		else {
			Connect_(endpoint_, BIND_WITH_1_ARG(Write_, message));
		}
	}

	void Cancel()
	{
		if (sock_.is_open()) {
			Write_(std::string{ MESSAGE_END });
		}
	}

	void Close()
	{
		sock_.close();
	}

private:
	typedef Transmitter SelfType;
	typedef boost::function<void()> OnConnect;

	boost::asio::ip::tcp::socket sock_;
	boost::asio::ip::tcp::endpoint const& endpoint_;
	ErrorHandler errorHandler_;

	Transmitter(boost::asio::io_service& service, boost::asio::ip::tcp::endpoint const& ep, ErrorHandler errorHandler)
		: sock_{ service }
		, endpoint_{ ep }
		, errorHandler_{ errorHandler }
	{}

	void Connect_(boost::asio::ip::tcp::endpoint const& ep, OnConnect onConnect = OnConnect())
	{
		sock_.async_connect(ep, BIND_WITH_2_ARGS(OnConnect_, onConnect, _1));
	}

	void OnConnect_(OnConnect onConnect, const boost::system::error_code& error)
	{
		if (error) {
			Close();
			errorHandler_(error);
			return;
		}
		if (onConnect) {
			onConnect();
		}
	}

	void Write_(std::string const& message)
	{
		bool isEnd{ message == std::string{ MESSAGE_END } };

		std::string copy{ message };
		if (!isEnd) {
			copy += COMMAND_END;
		}
		sock_.async_write_some(
			boost::asio::buffer(copy.c_str(), copy.size()),
			BIND_WITH_3_ARGS(OnWrite_, _1, _2, isEnd)
		);
	}

	void OnWrite_(boost::system::error_code const& error, size_t bytes, bool isEnd = false)
	{
		if (error) {
			Close();
			errorHandler_(error);
			return;
		}
		if (isEnd) {
			Close();
		}
	}
};



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

		CreateReceiver();
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

	void CreateReceiver()
	{
		Receiver::SharedPtr client = Receiver::Create(
			service_,
			BIND_WITH_1_ARG(CommandHandler, _1),
			BIND_WITH_1_ARG(ErrorHandler, _1)
		);
		acceptor_.async_accept(client->Sock(), BIND_WITH_2_ARGS(HandleAccept, client, _1));
	}

	void HandleAccept(Receiver::SharedPtr client, boost::system::error_code const& error)
	{
		client->Start();
		CreateReceiver();
	}

	void CommandHandler(std::string const& command)
	{
		std::cout << command << std::endl;
	}

	void ErrorHandler(boost::system::error_code const& error)
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
	Server::SharedPtr server{ Server::Create(4444) };
	server->Start();
}