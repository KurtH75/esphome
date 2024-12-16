#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/canbus/canbus.h"

namespace esphome {
namespace vscp {

class VscpLightOutput : public light::LightOutput, public canbus::Canbus, public Component {
 public:
  void setup() override;
  light::LightTraits get_traits() override;
  void set_output(output::FloatOutput *output) { output_ = output; }
  void write_state(light::LightState *state) override;
  void dump_config() override;
  void set_subzone(int8_t subzone_);
 
 protected:
  output::FloatOutput *output_;
};

} //namespace vscp
} //namespace esphome