#pragma once

#include <GxEPD2_BW.h>
#include <SPI.h>

#include "esphome/components/display/display.h"
#include "esphome/core/log.h"

namespace esphome::weact_epaper {

class WeActEpaper : public display::Display {
 public:
  WeActEpaper(int clk_pin, int mosi_pin, int cs_pin, int dc_pin, int reset_pin, int busy_pin)
      : clk_pin_(clk_pin),
        mosi_pin_(mosi_pin),
        cs_pin_(cs_pin),
        display_(GxEPD2_370_GDEY037T03(cs_pin, dc_pin, reset_pin, busy_pin)) {}

  void setup() override {
    SPI.begin(this->clk_pin_, -1, this->mosi_pin_, this->cs_pin_);
    this->display_.epd2.selectSPI(SPI, SPISettings(4000000, MSBFIRST, SPI_MODE0));
    this->display_.init(0, true, 50, false);
    this->display_.setRotation(1);
  }

  void update() override {
    this->display_.setFullWindow();
    this->display_.firstPage();
    do {
      this->display_.fillScreen(GxEPD_WHITE);
      this->do_update_();
    } while (this->display_.nextPage());
    this->display_.hibernate();
  }

  void draw_pixel_at(int x, int y, Color color) override {
    this->display_.drawPixel(x, y, color.is_on() ? GxEPD_BLACK : GxEPD_WHITE);
  }

  void dump_config() override { LOG_DISPLAY("", "WeAct GDEY037T03", this); }

  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_BINARY; }

 protected:
  int get_width_internal() override { return 416; }
  int get_height_internal() override { return 240; }

  int clk_pin_;
  int mosi_pin_;
  int cs_pin_;
  GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> display_;
};

}  // namespace esphome::weact_epaper
