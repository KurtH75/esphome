#include "vscp2.h"

#include "esphome/core/log.h"
#include "esphome/components/canbus/canbus.h"

#include <cinttypes>

namespace esphome {
namespace vscp2 {

static const char *TAG = "vscp2";

void vscp2Component::setup() {
	ESP_LOGCONFIG(TAG, "Setting up VSCP2 generation component...");

// 	uint8_t data[15];
// 	memset(data, 0, sizeof(data));
// 	data[0] = 0x0E;
// 	data[9] = 0x11;

// 	((CC2500Device*)this)->send(data, 15);
 	}

bool vscp2Component::receive(uint32_t can_id, bool rtr, std::vector<uint8_t> &data) {
	ESP_LOGV(TAG, "class1.INFORMATION event received: %x", can_id);
	if ((can_id & 0x00FFFF00) == 0x00140300) {
    ESP_LOGV(TAG, "turn_on event");
  	}

  	if ((can_id & 0x00FFFF00) == 0x00140400) {
    ESP_LOGV(TAG, "turn_off event");
    
	// Check if the address is handled by a device
	bool success = false;
	for (auto device : this->devices_) {
		if(device->receive(can_id, rts, data)
			success = true;
	}

	if(!success) {
		// If the address is not yet handled, log the address as detected
		ESP_LOGI(TAG, "Address detected: 0x%016" PRIX64, address);
		ESP_LOGI(TAG, "  payload: 0x%06" PRIX64, payload);
	}

	return true;
}

void vscp2Component::send(uint8_t *data, uint8_t length) {
	data[11] = this->serial_number_++;
	((CC2500Device*)this)->send(data, length);
}

void vscp2ClientComponent::set_parent(vscp2Component *parent) {
	this->parent_ = parent;
	this->parent_->add_device(this);
}

void vscp2ClientComponent::send_(uint64_t address, uint8_t *data, uint8_t length) {
	uint8_t length_ = 10 + length;
	uint8_t data_[length_];

	// Packet length
	data_[0] = 10 + length - 1;

	// Addresses
	data_[1] = uint8_t(address >> 8 * 7);
	data_[2] = uint8_t(address >> 8 * 6);
	data_[3] = uint8_t(address >> 8 * 5);
	data_[4] = uint8_t(address >> 8 * 4);
	data_[5] = uint8_t(address >> 8 * 3);
	data_[6] = uint8_t(address >> 8 * 2);
	data_[7] = uint8_t(address >> 8 * 1);
	data_[8] = uint8_t(address >> 8 * 0);

	// Fixed
	data_[9] = 0x11;

	// Command
	for(int j = 0; j < length; j++)
		data_[10+j] = data[j];

	for (int i = 0; i < this->send_repeats_; i++) {
		if(i > 0)
			esphome::delay(14);
		this->parent_->send(&data_[0], length_);
	}
}

void set_canbus(canbus::Canbus *canbus) {
  Automation<std::vector<uint8_t>, uint32_t, bool> *automation;
  LambdaAction<std::vector<uint8_t>, uint32_t, bool> *lambdaaction;
  canbus::CanbusTrigger *canbus_canbustrigger;

  this->canbus = canbus;

  canbus_canbustrigger = new canbus::CanbusTrigger(canbus, 0x00140000, 0x01fff800, true); // Filter out only 'INFO <7' events
  canbus_canbustrigger->set_component_source("canbus");
  App.register_component(canbus_canbustrigger);
  automation = new Automation<std::vector<uint8_t>, uint32_t, bool>(canbus_canbustrigger);
  auto cb = [this](std::vector<uint8_t> x, uint32_t can_id, bool remote_transmission_request) -> void {
    this->receive(can_id, remote_transmission_request, x);
  };
  lambdaaction = new LambdaAction<std::vector<uint8_t>, uint32_t, bool>(cb);
  automation->add_actions({lambdaaction});
}

}
}
