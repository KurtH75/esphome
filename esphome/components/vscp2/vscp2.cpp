#include "vscp2.h"

#include "esphome/core/log.h"
#include "esphome/components/canbus/canbus.h"
#include "esphome.h"

#include <cinttypes>

namespace esphome {
namespace vscp2 {

static const char *TAG = "vscp2";

void Vscp2Component::setup() {
	ESP_LOGCONFIG(TAG, "Setting up VSCP2 generation component...");

// 	uint8_t data[15];
// 	memset(data, 0, sizeof(data));
// 	data[0] = 0x0E;
// 	data[9] = 0x11;

// 	((CC2500Device*)this)->send(data, 15);
 	}

bool Vscp2Component::receive(uint32_t  vcommand, std::vector<uint8_t> &data) {
	ESP_LOGV(TAG, "VSCP event received from node: %x", (vcommand & 0xFF));
	bool success = false;
	if ((vcommand & 0x00FFFF00) == 0x00140900) {
    ESP_LOGI(TAG, "Node 0x%02X heartbeat", (vcommand & 0xFF));
	success = true;
  	}
	if ((vcommand & 0x00FFFF00) == 0x00140300) {
    ESP_LOGV(TAG, "Info_on event");
  	}

  	if ((vcommand & 0x00FFFF00) == 0x00140400) {
    ESP_LOGV(TAG, "Info_off event");
	}
    
	// Check if the address is handled by a device
	
	for (auto device : this->devices_) {
		if(device->receive(vcommand, data))
			success = true;
	}

	if(!success) {
		// If the address is not yet handled, log the address as detected
		ESP_LOGI(TAG, "VSCP message was not processed");
		//ESP_LOGI(TAG, "  payload: 0x%06" PRIX64, payload);
	}

	return true;
}

void Vscp2Component::send(uint32_t vcommand, std::vector<uint8_t> &data) {
	
	
	canbus->send_data(vcommand, true, data);  //sufficient?
	
}

void Vscp2ClientComponent::set_parent(Vscp2Component *parent) {
	this->parent_ = parent;
	this->parent_->add_device(this);
}

void Vscp2ClientComponent::send(uint32_t vcommand, std::vector<uint8_t> &data) {
	
	this->parent_->send(vcommand, data);
	}


void Vscp2Component::set_canbus(canbus::Canbus *canbus) {
  Automation<std::vector<uint8_t>, uint32_t, bool> *automation;
  LambdaAction<std::vector<uint8_t>, uint32_t, bool> *lambdaaction;
  canbus::CanbusTrigger *canbus_canbustrigger;

  this->canbus = canbus;

  canbus_canbustrigger = new canbus::CanbusTrigger(canbus, 0, 0, true); //permit all messages
  canbus_canbustrigger->set_component_source("canbus");
  App.register_component(canbus_canbustrigger);
  automation = new Automation<std::vector<uint8_t>, uint32_t, bool>(canbus_canbustrigger);
  auto cb = [this](std::vector<uint8_t> x, uint32_t can_id, bool remote_transmission_request) -> void {
    this-> receive(can_id, x);
	//on_frame(can_id, remote_transmission_request, x);
	};
  lambdaaction = new LambdaAction<std::vector<uint8_t>, uint32_t, bool>(cb);
  automation->add_actions({lambdaaction});
}

 void Vscp2Component::on_frame(uint32_t can_id, bool rtr, std::vector<uint8_t> &data) {
   receive(can_id, data);

 }

}
}
