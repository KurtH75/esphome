#include "esphome/core/log.h"
#include "vscp_light.h"
#include "esphome/components/canbus/canbus.h"

namespace esphome {
namespace vscp {

static const char *TAG = "vscp_light.light";

void VscpLightOutput::setup() {
   
}

light::LightTraits VscpLightOutput::get_traits() {
    auto traits = light::LightTraits();
    traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});

    //traits.set_supports_brightness(true);
    //traits.set_supports_rgb(false);
    //traits.set_supports_rgb_white_value(false);
    //traits.set_supports_color_temperature(false);

    return traits;
}

void VscpLightOutput::write_state(light::LightState *state) {
    bool lightstate;
    state->current_values_as_binary(&lightstate);
    if (lightstate) {
        
    } 
}

void VscpLightOutput::set_subzone(int8_t subzone_) {
    
}

void VscpLightOutput::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty custom light");
}

} //namespace vscp
} //namespace esphome