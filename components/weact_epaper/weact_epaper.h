#pragma once

#include <gdey/GxEPD2_370_GDEY037T03.h>
#include <SPI.h>

#include <cstring>

#include "esphome/components/display/display.h"
#include "esphome/core/log.h"

namespace esphome::weact_epaper {

static const char *const TAG = "weact_epaper";

class WeActEpaper : public display::Display {
 public:
  WeActEpaper(int clk_pin, int mosi_pin, int cs_pin, int dc_pin, int reset_pin, int busy_pin)
      : clk_pin_(clk_pin),
        mosi_pin_(mosi_pin),
        cs_pin_(cs_pin),
        panel_(cs_pin, dc_pin, reset_pin, busy_pin) {}

  void setup() override {
    SPI.begin(this->clk_pin_, -1, this->mosi_pin_, this->cs_pin_);
    this->panel_.selectSPI(SPI, SPISettings(4000000, MSBFIRST, SPI_MODE0));
    this->panel_.init(0, true, 50, false);
  }

  void update() override {
    std::memset(this->buffer_, 0xFF, sizeof(this->buffer_));
    this->do_update_();
    this->panel_.writeImageForFullRefresh(this->buffer_, 0, 0, NATIVE_WIDTH, NATIVE_HEIGHT);
    this->panel_.refresh(false);
    this->panel_.writeImageAgain(this->buffer_, 0, 0, NATIVE_WIDTH, NATIVE_HEIGHT);
    this->panel_.hibernate();
  }

  void draw_pixel_at(int x, int y, Color color) override {
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT)
      return;

    const int native_x = NATIVE_WIDTH - y - 1;
    const int native_y = x;
    const size_t index = native_x / 8 + native_y * (NATIVE_WIDTH / 8);
    const uint8_t mask = 0x80 >> (native_x % 8);
    if (color.is_on())
      this->buffer_[index] &= ~mask;
    else
      this->buffer_[index] |= mask;
  }

  void dump_config() override { LOG_DISPLAY("", "WeAct GDEY037T03", this); }

  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_BINARY; }

 protected:
  static constexpr int NATIVE_WIDTH = 240;
  static constexpr int NATIVE_HEIGHT = 416;
  static constexpr int DISPLAY_WIDTH = 416;
  static constexpr int DISPLAY_HEIGHT = 240;

  int get_width_internal() override { return DISPLAY_WIDTH; }
  int get_height_internal() override { return DISPLAY_HEIGHT; }

  int clk_pin_;
  int mosi_pin_;
  int cs_pin_;
  uint8_t buffer_[NATIVE_WIDTH / 8 * NATIVE_HEIGHT]{};
  GxEPD2_370_GDEY037T03 panel_;
};

}  // namespace esphome::weact_epaper
