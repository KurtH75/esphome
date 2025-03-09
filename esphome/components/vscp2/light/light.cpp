#include "esphome/core/log.h"
#include "light.h"
#include "../../light/light_state.h"

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
	//state_->set_restore_mode(LightRestoreMode.LIGHT_RESTORE_AND_OFF); //I can't get this to work.
}

void Vscp2Light::write_state(light::LightState *state) {
	if (this->receive_ == true) {
		ESP_LOGV(TAG, "Receive mode, don't write state to light");
		this->receive_ = false;
		return;
	}

	float brightness, pwm_value;
	state->current_values_as_brightness(&brightness);

	VSCPcommand vcommand;
	uint8_t hue, saturation, value;
	if (brightness > 0.0) {

		vcommand = VSCPcommand::EVENT_CONTROL_TURN_ON;
		if (this->dimmable_) {
			vcommand = VSCPcommand::EVENT_CHANGE_LEVEL;
			pwm_value = (uint8_t) lroundf(brightness * 255.0);
			ESP_LOGI(TAG, "Setting light on zone/subzone 0x%02X / 0x%02X to brightness %.4f -> PWM value 0x%02X", this->zone_, this->subzone_, brightness, pwm_value);


		}
		else {
			vcommand = VSCPcommand::EVENT_CONTROL_TURN_ON;
			ESP_LOGI(TAG, "Setting light on zone/subzone 0x%02X / 0x%02X to ON", this->zone_, this->subzone_);
		}
		
	} else {
		vcommand = VSCPcommand::EVENT_CONTROL_TURN_OFF;
		ESP_LOGI(TAG, "Setting light on zone/subzone 0x%02X / 0x%02X to OFF", this->zone_, this->subzone_);

	}

	
	std::vector<uint8_t> data(3);

	// VSCP can packet format
	data[0] = pwm_value;
	data[1] = this->zone_;
	data[2] = this->subzone_;
	
	this->send((uint32_t) vcommand, data);
}

bool Vscp2Light::receive(uint32_t vcommand, std::vector<uint8_t> &data) {
	// Command
	vcommand = vcommand & 0xFFFF00;
	
	// HSV Color values
	// uint8_t hue = data[2];
	// uint8_t saturation = data[3];
	// uint8_t value = data[4];

	//ESP_LOGD(TAG, "Received command 0x%02X for address 0x%016" PRIX64 " HSV: 0x%02X 0x%02X 0x%02X", command, this->address_, hue, saturation, value);

	// int _hue = int(round(float(hue) / (255.0 / 360.0)));
	// float _saturation = float(saturation) / 255;
	float _value;
	// float red, green, blue;
	// hsv_to_rgb(_hue, _saturation, _value, red, green, blue);

	if((vcommand == (uint32_t) VSCPcommand::EVENT_INFORMATION_OFF) && (zone_ == data[1]) && (subzone_ == data[2])) {
		ESP_LOGV(TAG, "Matched Event_Off with entity on zone %x and subzone %x", zone_, subzone_);
		this->receive_ = true;
		auto call = this->state_->turn_off();

		call.perform();
		return true;
	} else if((vcommand == (uint32_t) VSCPcommand::EVENT_INFORMATION_ON) && (zone_ == data[1]) && (subzone_ == data[2])) {
		ESP_LOGV(TAG, "Matched Event_On with entity on zone %x and subzone %x", zone_, subzone_);
		this->receive_ = true;

		auto call = this->state_->turn_on();
		// call.set_rgb(red, green, blue);
		// call.set_brightness(_value);
		// if(command == Command::ON)
		// 	call.set_effect("none");

		call.perform();
		return true;
	} else if((vcommand == (uint32_t) VSCPcommand::EVENT_INFORMATION_LEVEL) && (zone_ == data[1]) && (subzone_ == data[2])) {
		ESP_LOGV(TAG, "Matched Event_Level with entity on zone %x and subzone %x", zone_, subzone_);
		this->receive_ = true;
		_value = (float) (data[0]/255.0);

		auto call = this->state_->make_call();
		call.set_brightness(_value);

		call.perform();
		return true;
	}

	//ESP_LOGE(TAG, "Received unknown command 0x%02X", vcommand);
	return false;
}



}
}
