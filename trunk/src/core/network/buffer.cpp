#include <cstring>

#include <network/buffer.hpp>
#include <common/log.hpp>

namespace eMU {
namespace core {
namespace network {

readBuffer_t::readBuffer_t():
  payload_(maxPayloadSize_c, 0),
  payloadSize_(0) {}

void readBuffer_t::clear() {
    payload_.clear();
    payload_.resize(maxPayloadSize_c, 0);
    payloadSize_ = 0;
}

writeBuffer_t::writeBuffer_t():
  payload_(maxPayloadSize_c, 0),
  payloadSize_(0),
  pending_(false),
  secPayload_(maxPayloadSize_c, 0),
  secPayloadSize_(0) {}

void writeBuffer_t::clear() {
    this->clearPri();
    this->clearSec();
}

void writeBuffer_t::clearPri() {
    pending_ = false;

    payload_.clear();
    payload_.resize(maxPayloadSize_c, 0);
    payloadSize_ = 0;
}

void writeBuffer_t::clearSec() {
    secPayload_.clear();
    secPayload_.resize(maxPayloadSize_c, 0);
    secPayloadSize_ = 0;
}

bool writeBuffer_t::insert(const uint8 *payload, size_t size) {
    payload_t &destPayload = pending_ ? secPayload_ : payload_;
    size_t &destPayloadSize = pending_ ? secPayloadSize_ : payloadSize_;

    if((destPayloadSize + size) > maxPayloadSize_c) {
        LOG_ERROR << "buffer overflow! pending: " << pending_ 
                  << ", current size: " << destPayloadSize 
                  << ", size: " << size << std::endl;
        return false;
    }

    memcpy(&destPayload[destPayloadSize], payload, size);
    destPayloadSize += size;

    return true;
}

void writeBuffer_t::swap() {
    payload_ = secPayload_;
    payloadSize_ = secPayloadSize_;
    this->clearSec();
}

}
}
}