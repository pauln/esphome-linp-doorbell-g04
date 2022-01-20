#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

enum Szy8039bCommand { SZY8039B_COMMAND_PLAY = 0, SZY8039B_COMMAND_STOP };

struct Szy8039bData {
  uint8_t tune;
  uint8_t volume;
  uint8_t mode;
  Szy8039bCommand command;

  bool operator==(const Szy8039bData &rhs) const {
    return tune == rhs.tune
      && volume == rhs.volume
      && mode == rhs.mode
      && command == rhs.command;
  }
};

class Szy8039bProtocol : public RemoteProtocol<Szy8039bData> {
 public:
  void encode(RemoteTransmitData *dst, const Szy8039bData &data) override;
  optional<Szy8039bData> decode(RemoteReceiveData src) override;
  void dump(const Szy8039bData &data) override;
 protected:
  void encode_byte_(RemoteTransmitData *dst, uint8_t byte);
};

DECLARE_REMOTE_PROTOCOL(Szy8039b)

template<typename... Ts> class Szy8039bAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint8_t, tune)
  TEMPLATABLE_VALUE(uint8_t, volume)
  TEMPLATABLE_VALUE(uint8_t, mode)
  TEMPLATABLE_VALUE(Szy8039bCommand, command)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    Szy8039bData data{};
    data.tune = this->tune_.value(x...);
    data.volume = this->volume_.value(x...);
    data.mode = this->mode_.value(x...);
    data.command = this->command_.value(x...);
    Szy8039bProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
