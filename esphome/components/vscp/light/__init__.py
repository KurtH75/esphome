import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, output, canbus
from esphome.const import CONF_OUTPUT_ID, CONF_OUTPUT
from .. import vscp_ns

DEPENDENCIES = ['canbus']

VSCP_ZONE = 'zone'
VSCP_SUBZONE = 'subzone'

#vscp_ns = cg.esphome_ns.namespace('vscp') #already done in ../__init__.py ?
VscpLightOutput = vscp_ns.class_("VscpLightOutput", light.LightOutput, canbus.Canbus)

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(VscpLightOutput),
    cv.Optional(CONF_OUTPUT): cv.use_id(output.FloatOutput),
    cv.Optional(VSCP_ZONE, default=0x00): cv.int_,
    cv.Required(VSCP_SUBZONE): cv.int_
})

def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield light.register_light(var, config)

    out = yield cg.get_variable(config[CONF_OUTPUT])
    cg.add(var.set_output(out))
 #   cg.add(var.set_output(config[VSCP_SUBZONE]))
    cg.add(var.set_subzone(config[VSCP_SUBZONE]))