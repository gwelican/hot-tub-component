import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor"]

spa_ns = cg.esphome_ns.namespace("spa")
SpaReader = spa_ns.class_("SpaReader", cg.Component, uart.UARTDevice)

CONF_SPA_ID = "spa_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SpaReader),
        cv.GenerateID(uart.CONF_UART_ID): cv.use_id(uart.UARTComponent),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    uart_component = await cg.get_variable(config[uart.CONF_UART_ID])
    cg.add(var.set_uart(uart_component))
