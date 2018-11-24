#ifndef __TRANSCEIVER_INCLUDED__
#define __TRANSCEIVER_INCLUDED__

#include "General.hpp"
#include "Receiver.hpp"
#include "Transmitter.hpp"

namespace HockeyNet
{
	class Transceiver
		: public boost::enable_shared_from_this<Transceiver>
		, boost::noncopyable
	{
	public:
		static TransceiverPtr Create(boost::asio::io_service& service);
		void SetErrorHandler(OnErrorHandler onErrorHandler);
		void SetAnswerHandler(OnAnswerHandler onAnswerHandler);
		void SetCloseHandler(OnCloseHandler onCloseHandler);

		SocketPtr Sock();

		void Connect(boost::asio::ip::tcp::endpoint const& ep);
		void AsyncConnect(boost::asio::ip::tcp::endpoint const& ep);
		void StartReading();
		void Send(std::string const& message);
		void Close();

	private:
		typedef Transceiver SelfType;

		ReceiverPtr receiver_;
		TransmitterPtr transmitter_;

		Transceiver(boost::asio::io_service& service);
	};
};

#endif // __TRANSCEIVER_INCLUDED__
