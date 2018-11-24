#ifndef __TRANSMITTER_INCLUDED__
#define __TRANSMITTER_INCLUDED__

#include "General.hpp"

namespace HockeyNet
{
	class Transmitter
		: public boost::enable_shared_from_this<Transmitter>
		, boost::noncopyable
	{
	public:
		static TransmitterPtr Create(boost::asio::io_service& service);
		static TransmitterPtr Create(SocketPtr socket);
		void SetErrorHandler(OnErrorHandler onErrorHandler);
		void SetCloseHandler(OnCloseHandler onCloseHandler);

		SocketPtr Sock();

		void Connect(boost::asio::ip::tcp::endpoint const& ep);
		void AsyncConnect(boost::asio::ip::tcp::endpoint const& ep);
		void Send(std::string const& message);
		void Close();

	private:
		typedef Transmitter SelfType;
		friend class Transceiver;

		bool closed_;
		SocketPtr sock_;
		size_t transfersCount_;
		OnErrorHandler onErrorHandler_;
		OnCloseHandler onCloseHandler_;

		explicit Transmitter(boost::asio::io_service& service);
		explicit Transmitter(SocketPtr socket);

		void OnConnect_(const boost::system::error_code& error);

		void Write_(std::string const& message);
		void OnWrite_(boost::system::error_code const& error, size_t bytes, char lastChar);

		void SafeClose_(char endChar);
		void CloseSocket_();
	};
};

#endif // __TRANSMITTER_INCLUDED__
