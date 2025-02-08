import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_LIGHT_ID

from .. import (
    VSCP2_DEVICE_SCHEMA,
    vscp2_ns,
    register_vscp2_device,
)

DEPENDENCIES = ["vscp2"]
CODEOWNERS = ["@rrooggiieerr"]



Vscp2Light = vscp2_ns.class_(
    "Vscp2Light", cg.Component, light.LightOutput
)

CONFIG_SCHEMA = cv.All(
    light.LIGHT_SCHEMA.extend(
        {
            cv.GenerateID(CONF_LIGHT_ID): cv.declare_id(Vscp2Light),
        }
    )
    .extend(VSCP2_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_LIGHT_ID])
    
    await cg.register_component(var, config)
    await register_vscp2_device(var, config)
    await light.register_light(var, config)
