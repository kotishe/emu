#ifndef eMU_CORE_UDPCONNECTION_HPP
#define eMU_CORE_UDPCONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "buffer.hpp"
#include "udpWriteBufferManager.hpp"

#ifdef WIN32
#pragma warning(disable: 4275)
#pragma warning(disable: 4251)
#endif

namespace eMU {
namespace core {
namespace network {
namespace udp {

template<typename SocketImpl = boost::asio::ip::udp::socket,
         typename IoServiceImpl = boost::asio::io_service>
class connection_t: private boost::noncopyable {
public:
    typedef boost::function3<void, connection_t*, const boost::asio::ip::udp::endpoint&, payload_t&> receiveFromEventCallback_t;

    connection_t(IoServiceImpl &ioService, uint16 port):
      socket_(ioService,
              boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
      strand_(ioService) {}

    virtual ~connection_t() {}

    void queueReceiveFrom() {
        socket_.async_receive_from(boost::asio::buffer(&rbuf_.payload_[0], maxPayloadSize_c),
                                   senderEndpoint_,
                                   strand_.wrap(boost::bind(&connection_t::receiveFromHandler,
                                                            this,
                                                            boost::asio::placeholders::error,
                                                            boost::asio::placeholders::bytes_transferred)));
    }

    SocketImpl& socket() { return socket_; }

    void receiveFromEventCallback(const receiveFromEventCallback_t &callback) { receiveFromEventCallback_ = callback; }

    void sendTo(const boost::asio::ip::udp::endpoint &endpoint, uint8 *payload, size_t size) {
        writeBuffer_t *wbuf = wbufManager_.get(endpoint);

        bool result = wbuf->insert(payload, size);

        if(!result) {
            this->errorHandler(boost::asio::error::no_buffer_space, "sendTo");
            return;
        }

        if(!wbuf->pending_) {
            wbuf->pending_ = true;
            this->queueSendTo(endpoint, wbuf);
        }
    }

private:
    connection_t();

    void receiveFromHandler(const boost::system::error_code& ec, size_t bytesTransferred) {
       if(ec) {
            this->errorHandler(ec, "receiveFrom");
        } else {
            payload_t payload(rbuf_.payload_.begin(), rbuf_.payload_.begin() + bytesTransferred);
            receiveFromEventCallback_(this, senderEndpoint_, payload);
        }

        this->queueReceiveFrom();
    }

    void queueSendTo(const boost::asio::ip::udp::endpoint &endpoint, writeBuffer_t *wbuf) {
        // comment deleted by ACTA :-).
        socket_.async_send_to(boost::asio::buffer(&wbuf->payload_[0], wbuf->payloadSize_),
                              endpoint,
                              strand_.wrap(boost::bind(&connection_t::sendToHandler,
                                                       this,
                                                       endpoint,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::bytes_transferred))); 
    }

    void sendToHandler(const boost::asio::ip::udp::endpoint &endpoint,
                       const boost::system::error_code& ec,
                       size_t bytesTransferred) {
        if(ec) {
            this->errorHandler(ec, "sendTo");
        }

        writeBuffer_t *wbuf = wbufManager_.get(endpoint);

        if(wbuf->secPayloadSize_ > 0) {
            wbuf->swap();
            this->queueSendTo(endpoint, wbuf);
        }
        else
            wbufManager_.free(endpoint);
    }

    void errorHandler(const boost::system::error_code &ec, const std::string &operationName) {
        LOG_ERROR << "Error during handling async operation: " << operationName 
                  << ", error: " << ec.message() 
                  << ", code: " << ec.value() << std::endl;
    }

    SocketImpl socket_;
    readBuffer_t rbuf_;
    writeBufferManager_t wbufManager_;
    typename IoServiceImpl::strand strand_;
    boost::asio::ip::udp::endpoint senderEndpoint_;
    receiveFromEventCallback_t receiveFromEventCallback_;
};

}
}
}
}

#ifdef WIN32
#pragma warning(default: 4275)
#pragma warning(default: 4251)
#endif

#endif