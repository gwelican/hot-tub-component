import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID, CONF_NAME
from . import CONF_SPA_ID, BalboaSpa, balboa_spa_ns

DEPENDENCIES = ["balboa_spa"]

BalboaSpaButton = balboa_spa_ns.class_("BalboaSpaButton", button.Button, cg.Component)

# Button types
CONF_JET1_BUTTON = "jet1"
CONF_JET2_BUTTON = "jet2"
CONF_SOAK_BUTTON = "soak"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SPA_ID): cv.use_id(BalboaSpa),
    cv.Optional(CONF_JET1_BUTTON): button.BUTTON_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(BalboaSpaButton),
    }).extend(cv.COMPONENT_SCHEMA),
    cv.Optional(CONF_JET2_BUTTON): button.BUTTON_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(BalboaSpaButton),
    }).extend(cv.COMPONENT_SCHEMA),
    cv.Optional(CONF_SOAK_BUTTON): button.BUTTON_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(BalboaSpaButton),
    }).extend(cv.COMPONENT_SCHEMA),
})

async def to_code(config):
    spa = await cg.get_variable(config[CONF_SPA_ID])
    
    for key in [CONF_JET1_BUTTON, CONF_JET2_BUTTON, CONF_SOAK_BUTTON]:
        if key not in config:
            continue
        conf = config[key]
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await button.register_button(var, conf)
        cg.add(getattr(spa, f"set_{key}_button")(var)) 