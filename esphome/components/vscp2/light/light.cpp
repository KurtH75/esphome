#include "esphome/core/log.h"
#include "light.h"

#include <cinttypes>

namespace esphome {
namespace vscp2 {

static const char *TAG = "vscp2.light";

void Vscp2Light::setup() {
	ESP_LOGCONFIG(TAG, "Setting up VSCP light...");
}

void Vscp2Light::dump_config() {
	ESP_LOGCONFIG(TAG, "VSCP light:");
	ESP_LOGCONFIG(TAG, "  Zone: %x", this->zone_);
	ESP_LOGCONFIG(TAG, "  Subzone: %d", this->subzone_);
}

void Vscp2Light::setup_state(light::LightState *state) {
	state_ = state;
	state_->set_gamma_correct(0);
	state_->set_default_transition_length(0);
}

void Vscp2Light::write_state(light::LightState *state) {
	if (this->receive_ == true) {
		ESP_LOGV(TAG, "Receive mode, don't write state to light");
		this->receive_ = false;
		return;
	}

	float brightness;
	state->current_values_as_brightness(&brightness);

	VSCPcommand vcommand;
	uint8_t hue, saturation, value;
	if (brightness > 0.0) {
		float red, green, blue;
		state->current_values_as_rgb(&red, &green, &blue, false);

		int _hue;
		float _saturation, _value;
		rgb_to_hsv(red, green, blue, _hue, _saturation, _value);

		vcommand = VSCPcommand::EVENT_CONTROL_TURN_ON;

		hue = (uint8_t) lroundf(_hue * (255.0 / 360.0));
		saturation = (uint8_t) lroundf(_saturation * 255.0);
		value = (uint8_t) lroundf(_value * 255.0);
	} else {
		vcommand = VSCPcommand::EVENT_CONTROL_TURN_OFF;
		hue = 0;
		saturation = 0;
		value = 0;
	}

	ESP_LOGV(TAG, "Setting light on zone 0x%02X - subzone 0x%02X to 0x%02X 0x%02X 0x%02X 0x%02X", this->zone_, this->subzone_,  (uint8_t) vcommand, hue, value);

	uint8_t data[3];

	// VSCP can packet format
	data[0] = 0;
	data[1] = this->zone_;
	data[2] = this->subzone_;
	
	this->send((uint32_t) vcommand, data);
}

bool Vscp2Light::receive(uint32_t vcommand, std::vector<uint8_t> *data) {
	// Command
	vcommand = vcommand & 0xFFFF00;
	
	// HSV Color values
	// uint8_t hue = data[2];
	// uint8_t saturation = data[3];
	// uint8_t value = data[4];

	//ESP_LOGD(TAG, "Received command 0x%02X for address 0x%016" PRIX64 " HSV: 0x%02X 0x%02X 0x%02X", command, this->address_, hue, saturation, value);

	// int _hue = int(round(float(hue) / (255.0 / 360.0)));
	// float _saturation = float(saturation) / 255;
	// float _value = float(value) / 255;
	// float red, green, blue;
	// hsv_to_rgb(_hue, _saturation, _value, red, green, blue);

	if((vcommand == (uint32_t) VSCPcommand::EVENT_INFORMATION_OFF) && (zone_ == data[1]) && (subzone_ == data[2])) {
		this->receive_ = true;
		auto call = this->state_->turn_off();

		call.perform();
		return true;
	} else if((vcommand == (uint32_t) VSCPcommand::EVENT_INFORMATION_ON) && (zone_ == data[1]) && (subzone_ == data[2])) {
		this->receive_ = true;

		auto call = this->state_->turn_on();
		// call.set_rgb(red, green, blue);
		// call.set_brightness(_value);
		// if(command == Command::ON)
		// 	call.set_effect("none");

		call.perform();
		return true;
	} 

	ESP_LOGE(TAG, "Received unknown command 0x%02X", vcommand);
	return false;
}



}
}
