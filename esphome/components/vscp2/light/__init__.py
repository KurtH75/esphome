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


LightRestoreMode = light_ns.enum("LightRestoreMode")
RESTORE_MODES = {
    "RESTORE_DEFAULT_OFF": LightRestoreMode.LIGHT_RESTORE_DEFAULT_OFF,
    "RESTORE_DEFAULT_ON": LightRestoreMode.LIGHT_RESTORE_DEFAULT_ON,
    "ALWAYS_OFF": LightRestoreMode.LIGHT_ALWAYS_OFF,
    "ALWAYS_ON": LightRestoreMode.LIGHT_ALWAYS_ON,
    "RESTORE_INVERTED_DEFAULT_OFF": LightRestoreMode.LIGHT_RESTORE_INVERTED_DEFAULT_OFF,
    "RESTORE_INVERTED_DEFAULT_ON": LightRestoreMode.LIGHT_RESTORE_INVERTED_DEFAULT_ON,
    "RESTORE_AND_OFF": LightRestoreMode.LIGHT_RESTORE_AND_OFF,
    "RESTORE_AND_ON": LightRestoreMode.LIGHT_RESTORE_AND_ON,
}

LIGHT_SCHEMA = (
    cv.ENTITY_BASE_SCHEMA.extend(web_server.WEBSERVER_SORTING_SCHEMA)
    .extend(cv.MQTT_COMMAND_COMPONENT_SCHEMA)
    .extend(
        {
            cv.GenerateID(): cv.declare_id(LightState),
            cv.OnlyWith(CONF_MQTT_ID, "mqtt"): cv.declare_id(
                mqtt.MQTTJSONLightComponent
            ),
            cv.Optional(CONF_RESTORE_MODE, default="ALWAYS_OFF"): cv.enum(
                RESTORE_MODES, upper=True, space="_"
            ),
        }
    )
)


Vscp2Light = vscp2_ns.class_(
    "Vscp2Light", cg.Component, light.LightOutput
)

CONFIG_SCHEMA = cv.All(
    light.LIGHT_SCHEMA.extend(
        {
            cv.GenerateID(CONF_LIGHT_ID): cv.declare_id(Vscp2Light),
            cv.Optional(CONF_RESTORE_MODE, default="ALWAYS_OFF"): cv.enum(RESTORE_MODES, upper=True, space="_"),
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
     cg.add(var.set_restore_mode(config[CONF_RESTORE_MODE]))
