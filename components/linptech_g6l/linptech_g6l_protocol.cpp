#include "linptech_g6l_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.linptech_g6l";

static const uint32_t HEADER_HIGH_US = 43;
static const uint32_t HEADER_LOW_US = 1000;
static const uint32_t BIT_ONE_HIGH_US = 97;
static const uint32_t BIT_ONE_LOW_US = 36;
static const uint32_t BIT_ZERO_HIGH_US = 36;
static const uint32_t BIT_ZERO_LOW_US = 97;

void LinptechG6lProtocol::encode(RemoteTransmitData *dst, const LinptechG6lData &data) {
  dst->reserve(138);
  dst->set_carrier_frequency(43392);

  // HEADER
  dst->item(HEADER_HIGH_US, HEADER_LOW_US);

  //  The buttons usually manage to transmit 3-4 times on a single press.
  for (uint i = 0; i < 4; i++) {
    // Address (MSB)
    for (uint32_t mask = 0x800000; mask != 0; mask >>= 1) {
      if (data.address & mask)
        dst->item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US);
      else
        dst->item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US);
    }
  }
}
optional<LinptechG6lData> LinptechG6lProtocol::decode(RemoteReceiveData src) {
  LinptechG6lData data{
      .address = 0,
  };

  // We don't see a real sync pulse, so just detect a valid bit.
  // Only peek it, so we can still decode it below.
  if (!(src.peek_item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US) || src.peek_item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US)))
    return {};

  ESP_LOGVV(TAG, "Potential button press detected");

  // Attempt to decode a 24-bit (3-byte) ID.
  for (uint32_t mask = 0x800000; mask != 0; mask >>= 1) {
    if (src.expect_item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US)) {
      data.address |= mask;
      ESP_LOGVV(TAG, "1 detected");
    } else if (src.expect_item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US)) {
      data.address &= ~mask;
      ESP_LOGVV(TAG, "0 detected");
    } else {
      ESP_LOGVV(TAG, "invalid bit detected");
      return {};
    }
  }

  // Expect a repeat.  Reject if not present.
  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return {};

  return data;
}

void LinptechG6lProtocol::dump(const LinptechG6lData &data) {
  ESP_LOGD(TAG, "Received Linptech G6L: address=0x%06X", data.address);
}

}  // namespace remote_base
}  // namespace esphome
