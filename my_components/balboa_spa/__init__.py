import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "switch", "number"]
CODEOWNERS = ["@pvarsanyi"]
CONF_SPA_ID = "spa_id"

# Create the namespace
balboa_spa_ns = cg.esphome_ns.namespace("balboa_spa")
BalboaSpa = balboa_spa_ns.class_("BalboaSpa", cg.Component)

CONF_UART_ID = "uart_id"
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SPA_ID): cv.declare_id(BalboaSpa),
        cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    """Generate the code for the spa reader component."""
    cg.add_library("rlogiacco/CircularBuffer", "1.4.0")

    var = cg.new_Pvariable(config[CONF_SPA_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)