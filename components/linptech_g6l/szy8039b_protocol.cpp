#include "szy8039b_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.szy8039b";

static const uint32_t HEADER_HIGH_US = 6000;
static const uint32_t HEADER_LOW_US = 500;
static const uint32_t BIT_ONE_HIGH_US = 1500;
static const uint32_t BIT_ONE_LOW_US = 500;
static const uint32_t BIT_ZERO_HIGH_US = 500;
static const uint32_t BIT_ZERO_LOW_US = 1500;

void Szy8039bProtocol::encode(RemoteTransmitData *dst, const Szy8039bData &data) {
  dst->reserve(138);

  if (data.command == SZY8039B_COMMAND_STOP) {
    // For a STOP command, just send it - no tune, volume, etc.
    ESP_LOGD(TAG, "Sending SZY8039B: command=STOP");
    this->encode_byte_(dst, 0xFF);

    return;
  }

  // Otherwise, assume it's a PLAY command.
  ESP_LOGD(TAG, "Sending SZY8039B: command=PLAY tune=%d volume=%d mode=%d", data.tune, data.volume, data.mode);

  // Tune must be sent first.
  uint8_t tune = data.tune - 1;
  this->encode_byte_(dst, tune);
  
  // Send volume (1 - 8 == 0xF0 - 0xF7).
  uint8_t volume = 0xF0 | (data.volume - 1);
  this->encode_byte_(dst, volume);

  // Send operation mode (1 - 4 == 0xE8 - 0xEB).
  uint8_t mode = 0xE7 + data.mode;
  this->encode_byte_(dst, mode);
  
  // Send "play" command.
  this->encode_byte_(dst, 0xFB);
}

void Szy8039bProtocol::encode_byte_(RemoteTransmitData *dst, uint8_t byte) {
  dst->space(HEADER_LOW_US);
  dst->mark(HEADER_HIGH_US);
  
  for (uint8_t mask = 0x01; mask != 0; mask <<= 1) {
    if (byte & mask) {
      dst->space(BIT_ONE_LOW_US);
      dst->mark(BIT_ONE_HIGH_US);
    } else {
      dst->space(BIT_ZERO_LOW_US);
      dst->mark(BIT_ZERO_HIGH_US);
    }
  }
}


optional<Szy8039bData> Szy8039bProtocol::decode(RemoteReceiveData src) {
  // Not implemented.
  return {};
}

void Szy8039bProtocol::dump(const Szy8039bData &data) {
  ESP_LOGD(TAG, "Received SZY8039B: tune=%d volume=%d mode=%d", data.tune, data.volume, data.mode);
}

}  // namespace remote_base
}  // namespace esphome
