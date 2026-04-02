import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID

from . import LD2410S

CONF_HAS_TARGET = "has_target"
CONF_LAST_COMMAND_SUCCESS = "last_command_success"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.use_id(LD2410S),

    cv.Optional(CONF_HAS_TARGET): binary_sensor.binary_sensor_schema(),
    cv.Optional(CONF_LAST_COMMAND_SUCCESS): binary_sensor.binary_sensor_schema(),
})


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ID])

    # 人体存在
    if CONF_HAS_TARGET in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_HAS_TARGET])
        cg.add(parent.set_has_target(sens))

    # 命令执行结果
    if CONF_LAST_COMMAND_SUCCESS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_LAST_COMMAND_SUCCESS])
        cg.add(parent.set_last_command_success(sens))
