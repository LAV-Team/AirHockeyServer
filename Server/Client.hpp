#ifndef __CLIENT_INCLUDED__
#define __CLIENT_INCLUDED__

#include "../Classes/Transceiver.hpp"

struct Client
{
public:
	typedef boost::shared_ptr<Client> ClientPtr;
	static ClientPtr Create(boost::asio::io_service& service);
	void SetErrorHandler(OnErrorHandler onErrorHandler);
	void SetAnswerHandler(OnAnswerHandler onAnswerHandler);
	void SetCloseHandler(OnCloseHandler onCloseHandler);

	TransceiverPtr GetTransceiver();
	std::string& GetSessionId();
	std::string GetSessionId() const;

	void SetSessionId(std::string sessionId);

	TransceiverPtr operator->();
	operator bool();

	boost::asio::ip::tcp::socket& Sock();
	void StartReading();
	void Send(std::string const& message);
	void Close();

private:
	TransceiverPtr transceiver_;
	std::string sessionId_;

	Client(boost::asio::io_service& service);
};

#endif // __CLIENT_INCLUDED__
