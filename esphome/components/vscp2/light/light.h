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

class Vscp2Light: public Vscp2ClientComponent, public light::LightOutput {
public:
	void setup() override;

	void dump_config() override;
	void setup_state(light::LightState *state) override;
	void write_state(light::LightState *state) override;
	light::LightTraits get_traits() override {
		auto traits = light::LightTraits();
		traits.set_supported_color_modes( { light::ColorMode::RGB });
		return traits;
	}
	bool receive(uint64_t address, uint8_t *data, uint8_t length) override;

protected:
	light::LightState *state_ { nullptr };
	bool receive_ = false;
};

class Vscp2CycleLightEffect : public light::LightEffect {
public:
	Vscp2CycleLightEffect()
	: light::LightEffect("Color Cycle") {}

	/// Called when this effect is about to be removed
	void stop() override;

	/// Apply this effect. Use the provided state for starting transitions, ...
	void apply() override;

	/// Internal method called by the LightState when this light effect is registered in it.
	void init() {}
protected:
	bool applied_ = false;
};

}
}
