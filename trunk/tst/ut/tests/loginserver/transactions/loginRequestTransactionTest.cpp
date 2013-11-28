#include <loginserver/transactions/loginRequestTransaction.hpp>

#include <protocol/readStream.hpp>
#include <protocol/dataserver/decoders/checkAccountRequest.hpp>
#include <protocol/dataserver/messageIds.hpp>
#include <protocol/loginserver/encoders/loginRequest.hpp>

#include <ut/env/core/network/tcp/connectionMock.hpp>

#include <gtest/gtest.h>
#include <boost/locale.hpp>

using ::testing::_;
using ::testing::Return;
using ::testing::SaveArg;

using eMU::ut::env::core::network::tcp::ConnectionMock;
using eMU::core::network::Payload;
using eMU::protocol::ReadStream;
namespace encoders = eMU::protocol::loginserver::encoders;
namespace decoders = eMU::protocol::loginserver::decoders;
using eMU::loginserver::transactions::LoginRequestTransaction;
using eMU::protocol::dataserver::decoders::CheckAccountRequest;
namespace MessageIds = eMU::protocol::dataserver::MessageIds;

class LoginRequestTransactionTest: public ::testing::Test
{
protected:
    LoginRequestTransactionTest():
        hash_(0x54321),
        accountId_(L"testAccount"),
        password_(L"testPassword"),
        request_(ReadStream(encoders::LoginRequest(accountId_,
                                                   password_).getWriteStream().getPayload())) {}

    size_t hash_;
    std::wstring accountId_;
    std::wstring password_;
    decoders::LoginRequest request_;
    ConnectionMock dataserverConnection_;
};

TEST_F(LoginRequestTransactionTest, handle)
{
    Payload payload;
    EXPECT_CALL(dataserverConnection_, send(_)).WillOnce(SaveArg<0>(&payload));

    bool connectionStatus = true;
    EXPECT_CALL(dataserverConnection_, isOpen()).WillOnce((Return(connectionStatus)));

    LoginRequestTransaction(hash_, dataserverConnection_, request_).handle();

    ReadStream readStream(payload);
    ASSERT_EQ(MessageIds::kCheckAccountRequest, readStream.getId());

    CheckAccountRequest checkAccountRequest(readStream);
    ASSERT_EQ(hash_, checkAccountRequest.getClientHash());
    ASSERT_EQ(boost::locale::conv::utf_to_utf<std::string::value_type>(accountId_), checkAccountRequest.getAccountId());
    ASSERT_EQ(boost::locale::conv::utf_to_utf<std::string::value_type>(password_), checkAccountRequest.getPassword());
}
