import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID
from . import LD2410S

CONF_OUTPUT_MODE = "output_mode"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.use_id(LD2410S),
    cv.Optional(CONF_OUTPUT_MODE): text_sensor.text_sensor_schema(),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ID])

    if CONF_OUTPUT_MODE in config:
        sens = await text_sensor.new_text_sensor(config[CONF_OUTPUT_MODE])
        cg.add(parent.set_output_mode(sens))
