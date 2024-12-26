#include "esphome/core/log.h"
#include "vscp_light.h"
#include "esphome/components/canbus/canbus.h"

#include "esphome.h"

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
      canbus->send_data(0x001e0500, true, false, {0x00, 0x00, 0xD3});  //turn subzone 0xD3 on
    } 
    else canbus->send_data(0x001e0600, true, false, {0x00, 0x00, 0xD3});  //turn subzone 0xD3 off
}

void VscpLightOutput::set_subzone(int8_t subzone_) {
    
}

void VscpLightOutput::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty custom light");
}

void VscpLightOutput::set_canbus(canbus::Canbus *canbus) {
  Automation<std::vector<uint8_t>, uint32_t, bool> *automation;
  LambdaAction<std::vector<uint8_t>, uint32_t, bool> *lambdaaction;
  canbus::CanbusTrigger *canbus_canbustrigger;

  this->canbus = canbus;

  canbus_canbustrigger = new canbus::CanbusTrigger(canbus, 0, 0, false);
  canbus_canbustrigger->set_component_source("canbus");
  App.register_component(canbus_canbustrigger);
  automation = new Automation<std::vector<uint8_t>, uint32_t, bool>(canbus_canbustrigger);
  auto cb = [this](std::vector<uint8_t> x, uint32_t can_id, bool remote_transmission_request) -> void {
    this->on_frame(can_id, remote_transmission_request, x);
  };
  lambdaaction = new LambdaAction<std::vector<uint8_t>, uint32_t, bool>(cb);
  automation->add_actions({lambdaaction});
}

void VscpLightOutput::on_frame(uint32_t can_id, bool rtr, std::vector<uint8_t> &data) {
  // recv_frame = {{can_id, {}, (uint8_t) data.size()}};
  // memcpy(recv_frame.value().Data, &data[0], data.size());
  // CONodeProcess(&node);
}


} //namespace vscp
} //namespace esphome