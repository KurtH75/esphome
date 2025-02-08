#pragma once

#include "esphome/core/color.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/base_light_effects.h"
#include "esphome/components/light/esp_hsv_color.h"
#include "esphome/components/vscp2/vscp2.h"

namespace esphome {
namespace vscp2 {

enum LightRestoreMode {
  LIGHT_RESTORE_DEFAULT_OFF,
  LIGHT_RESTORE_DEFAULT_ON,
  LIGHT_ALWAYS_OFF,
  LIGHT_ALWAYS_ON,
  LIGHT_RESTORE_INVERTED_DEFAULT_OFF,
  LIGHT_RESTORE_INVERTED_DEFAULT_ON,
  LIGHT_RESTORE_AND_OFF,
  LIGHT_RESTORE_AND_ON,
};

class Vscp2Light: public Vscp2ClientComponent, public light::LightOutput {
public:
	void setup() override;

	void dump_config() override;
	void setup_state(light::LightState *state) override;
	void write_state(light::LightState *state) override;
	light::LightTraits get_traits() override {
		auto traits = light::LightTraits();
		traits.set_supported_color_modes( { light::ColorMode::ON_OFF });
		return traits;
	}
	bool receive(uint32_t vcommand, std::vector<uint8_t> &data) override;

protected:
	light::LightState *state_ { nullptr };
	bool receive_ = false;
};


}
}
