#include <protocol/dataserver/checkAccountRequest.hpp>
#include <protocol/dataserver/messageIds.hpp>

namespace eMU
{
namespace protocol
{
namespace dataserver
{

CheckAccountRequest::CheckAccountRequest(const ReadStream &readStream):
    readStream_(readStream)
{
    clientHash_ = readStream_.readNext<core::network::tcp::NetworkUser::Hash>();

    uint32_t accountIdLength = readStream_.readNext<uint32_t>();
    accountId_ = readStream_.readNextString(accountIdLength);

    uint32_t passwordLength = readStream_.readNext<uint32_t>();
    password_ = readStream_.readNextString(passwordLength);
}

CheckAccountRequest::CheckAccountRequest(core::network::tcp::NetworkUser::Hash clientHash, const std::string &accountId, const std::string password):
    writeStream_(MessageIds::kCheckAccountRequest)
{
    writeStream_.writeNext<core::network::tcp::NetworkUser::Hash>(clientHash);

    writeStream_.writeNext<uint32_t>(accountId.length());
    writeStream_.writeNext(accountId);

    writeStream_.writeNext<uint32_t>(password.length());
    writeStream_.writeNext(password);
}

const WriteStream& CheckAccountRequest::getWriteStream() const
{
    return writeStream_;
}

core::network::tcp::NetworkUser::Hash CheckAccountRequest::getClientHash() const
{
    return clientHash_;
}

const std::string& CheckAccountRequest::getAccountId() const
{
    return accountId_;
}

const std::string& CheckAccountRequest::getPassword() const
{
    return password_;
}

}
}
}
