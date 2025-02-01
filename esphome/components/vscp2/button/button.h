#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/button/button.h"
#include "../vscp2.h"

namespace esphome {
namespace vscp2 {

class Vscp2ButtonComponent: public Vscp2ClientComponent, public button::Button {
public:
	void setup() override;
	void dump_config() override;
	bool receive(uint64_t address, uint8_t *data, uint8_t length) { return false; };

protected:
};

class Vscp2PairButton: public Vscp2ButtonComponent {
public:
	bool receive(uint64_t address, uint8_t *data, uint8_t length) override;

protected:
	void press_action() override;
};

class Vscp2UnpairButton: public Vscp2ButtonComponent {
protected:
	void press_action() override;
};

class Vscp2TestButton: public Vscp2ButtonComponent {
public:
protected:
	void press_action() override;
	uint8_t command = 0x00;
};

}  // namespace vscp2
}  // namespace esphome
