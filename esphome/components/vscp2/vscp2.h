#pragma once

#include "esphome/core/component.h"
#include "esphome/components/canbus/canbus.h"

namespace esphome {
namespace vscp2 {

enum class Command : uint8_t {
	PAIRING_REQUEST = 0x01,
	PAIRING_RESPONSE = 0x02,
	HSV = 0x03,
	ACK_HSV = 0x04,
	ON = 0x05,
	ACK_ON = 0x06,
	OFF = 0x07,
	ACK_OFF = 0x08,
	UNPAIRING_REQUEST = 0x0A,
	CYCLE = 0x0C,
	ACK_CYCLE = 0x0D,
	CYCLE_SYNC = 0x12,
};

class vscp2ClientComponent;

//class vscp2Component: public Component, public cc2500::CC2500Device<0, 3, 14> {
class vscp2Component: public Component {
public:
	void setup() override;

//	void dump_config() override;
	bool receive(uint8_t *data, uint8_t length) override;

	void add_device(vscp2ClientComponent *device) { this->devices_.push_back(device); }
	void send(uint8_t *data, uint8_t length);
	
	canbus::Canbus *canbus;
  	void set_canbus(canbus::Canbus *canbus);
  	void on_frame(uint32_t can_id, bool rtr, std::vector<uint8_t> &data);


protected:
	std::vector<vscp2ClientComponent *> devices_;

	uint8_t serial_number_ = 0;
};

class vscp2ClientComponent: public Component {
public:
	void set_parent(vscp2Component *parent);
	void set_address(uint64_t address) {
		this->address_ = address;
	}
	void set_send_repeats(uint16_t n_times) {
		this->send_repeats_ = n_times;
	}
	void send(uint8_t *data, uint8_t length) {
		this->send_(this->address_, &data[0], length);
	}
	virtual bool receive(uint64_t address, uint8_t *data, uint8_t length);

protected:
	vscp2Component *parent_ { nullptr };
	uint64_t address_;
	uint16_t send_repeats_ = 7;

	void send_(uint64_t address, uint8_t *data, uint8_t length);
	uint64_t swapped_address_() { return ((0x00000000FFFFFFFF & this->address_)<< 32) | ((0xFFFFFFFF00000000 & this->address_) >> 32); }
	uint64_t special_address_() { return 0xFFFFFFFF00000000 | (0x00000000FFFFFFFF & this->address_); }
	bool is_address_(uint64_t address) { return address == this->address_; }
	// A response has the light address and remote address swapped.
	bool is_response_(uint64_t address) { return address == this->swapped_address_(); }
	// If the first 4 bytes of the address are all 0xFF it seems to be a special address for
	// pairing and color cycling (and more?).
	bool is_special_(uint64_t address) { return address == this->special_address_(); }
};

}
}
