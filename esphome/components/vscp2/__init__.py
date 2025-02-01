import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import canbus
from esphome.components.canbus import CanbusComponent
from esphome.const import CONF_ID

DEPENDENCIES = []   #["cc2500"] 
CODEOWNERS = []

vscp2_ns = cg.esphome_ns.namespace("vscp2")
Vscp2Component = vscp2_ns.class_(
    "Vscp2Component", cg.Component  #"Vscp2Component", cc2500.CC2500Device, cg.Component
)


CONF_VSCP2_ID = "vscp2_id"
CONF_ADDRESS = "address"
CONF_SEND_REPEATS = "send_repeats"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Vscp2Component),
        }
    )
    # .extend(cc2500.CC2500_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
    #await cc2500.register_cc2500_device(var, config)


# A schema to use for all CC2500 devices, all CC2500 integrations must extend this!
VSCP2_DEVICE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VSCP2_ID): cv.use_id(Vscp2Component),
        cv.Required(CONF_ADDRESS): cv.hex_uint64_t,
        cv.Optional(CONF_SEND_REPEATS): cv.positive_int,
    }
)


async def register_vscp2_device(var, config):
    parent = await cg.get_variable(config[CONF_VSCP2_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    if CONF_SEND_REPEATS in config:
        cg.add(var.set_send_repeats(config[CONF_SEND_REPEATS]))
