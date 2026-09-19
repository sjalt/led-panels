import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import (
    CONF_BUSY_PIN,
    CONF_CLK_PIN,
    CONF_CS_PIN,
    CONF_DC_PIN,
    CONF_ID,
    CONF_LAMBDA,
    CONF_MOSI_PIN,
    CONF_RESET_PIN,
)

AUTO_LOAD = ["display"]
DEPENDENCIES = ["esp32"]

weact_epaper_ns = cg.esphome_ns.namespace("weact_epaper")
WeActEpaper = weact_epaper_ns.class_("WeActEpaper", display.Display)
WeActEpaperRef = WeActEpaper.operator("ref")

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(WeActEpaper),
        cv.Required(CONF_CLK_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_MOSI_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_CS_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_DC_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_RESET_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_BUSY_PIN): pins.internal_gpio_input_pin_number,
    }
).extend(cv.polling_component_schema("never"))


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_CLK_PIN],
        config[CONF_MOSI_PIN],
        config[CONF_CS_PIN],
        config[CONF_DC_PIN],
        config[CONF_RESET_PIN],
        config[CONF_BUSY_PIN],
    )
    await display.register_display(var, config)

    if lambda_config := config.get(CONF_LAMBDA):
        lambda_ = await cg.process_lambda(
            lambda_config, [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    cg.add_library("SPI", None)
    cg.add_library("Wire", None)
    cg.add_library("adafruit/Adafruit BusIO", None)
    cg.add_library("adafruit/Adafruit GFX Library", None)
    cg.add_library("zinggjm/GxEPD2", "1.6.5")
