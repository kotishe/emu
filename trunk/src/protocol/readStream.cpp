#include <protocol/readStream.hpp>

#include <string.h>

namespace eMU
{
namespace protocol
{

ReadStream::ReadStream(const core::network::Payload &payload):
    payload_(payload),
    currentOffset_(6)
{
}

uint16_t ReadStream::getId() const
{
    return this->readFromOffset<uint16_t>(4);
}

std::string ReadStream::readNextString(size_t length)
{
    std::string value;

    for(size_t i = 0; i < length; ++i)
    {
        value.push_back(this->readNext<std::string::value_type>());
    }

    return std::move(value);
}

std::wstring ReadStream::readNextWideString(size_t length)
{
    std::wstring value;

    for(size_t i = 0; i < length; ++i)
    {
        value.push_back(this->readNext<std::wstring::value_type>());
    }

    return std::move(value);
}

}
}