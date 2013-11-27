#pragma once

#include <mt/env/asioStub/types.hpp>
#include <mt/env/asioStub/ioService.hpp>

#include <boost/asio.hpp>
#include <gmock/gmock.h>

namespace eMU
{
namespace mt
{
namespace env
{
namespace asioStub
{
namespace ip
{
namespace tcp
{

class socket;

class acceptor
{
public:
    acceptor(io_service &ioService, const boost::asio::ip::tcp::endpoint &endpoint);
    void async_accept(socket &socket, const AcceptHandler &handler);

    io_service& ioService_;
};

}
}
}
}
}
}