#pragma once

#include "esphome/core/component.h"
#include "esphome/components/canbus/canbus.h"

namespace esphome {
namespace vscp2 {

//VSCP Can bus message classes and types mapped on the can extended id.
enum class VSCPcommand : uint32_t {
	
	EVENT_INFORMATION_ON = 0x140300,
	EVENT_INFORMATION_OFF = 0x140400,
	EVENT_INFORMATION_HEARTBEAT = 0x140900,
	EVENT_INFORMATION_LEVEL = 0x142800,

	EVENT_CONTROL_TURN_ON = 0x1e0500,
	EVENT_CONTROL_TURN_OFF = 0x1e0600,
	EVENT_CHANGE_LEVEL = 0x1e1600,
};

class Vscp2ClientComponent;

//class vscp2Component: public Component, public cc2500::CC2500Device<0, 3, 14> {
class Vscp2Component: public Component {
public:
	void setup() override;

//	void dump_config() override;
	//bool receive(uint8_t *data, uint8_t length) override;
	bool receive(uint32_t  vcommand, std::vector<uint8_t> &data);

	void add_device(Vscp2ClientComponent *device) { this->devices_.push_back(device); }
//	void send(uint8_t *data, uint8_t length);
	void send(uint32_t vcommand, std::vector<uint8_t> &data);
	
	canbus::Canbus *canbus;
  	void set_canbus(canbus::Canbus *canbus);
  	void on_frame(uint32_t can_id, bool rtr, std::vector<uint8_t> &data);


protected:
	std::vector<Vscp2ClientComponent *> devices_;
	uint8_t vnode_id_bridge_ = 0;
};

class Vscp2ClientComponent: public Component {
public:
	void set_parent(Vscp2Component *parent);
	void set_zone(uint8_t zone) {
		this->zone_ = zone;
	}
	void set_subzone(uint8_t subzone) {
		this->subzone_ = subzone;
	}
	void send(uint32_t vcommand, uint8_t *data) {
		this->send_(vcommand, &data);
	}
	virtual bool receive(uint64_t address, uint8_t *data, uint8_t length);

protected:
	Vscp2Component *parent_ { nullptr };
	uint8_t zone_;
	uint8_t subzone_;
	
	void send_(uint32_t vcommand, uint8_t *data);
	
};

}
}
