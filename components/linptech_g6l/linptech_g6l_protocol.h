#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

struct LinptechG6lData {
  uint32_t address;

  bool operator==(const LinptechG6lData &rhs) const { return address == rhs.address; }
};

class LinptechG6lProtocol : public RemoteProtocol<LinptechG6lData> {
 public:
  void encode(RemoteTransmitData *dst, const LinptechG6lData &data);
  optional<LinptechG6lData> decode(RemoteReceiveData src);
  void dump(const LinptechG6lData &data);
};

DECLARE_REMOTE_PROTOCOL(LinptechG6l)

template<typename... Ts> class LinptechG6lAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint32_t, address)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    LinptechG6lData data{};
    data.address = this->address_.value(x...);
    LinptechG6lProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
