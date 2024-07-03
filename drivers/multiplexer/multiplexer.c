/*
 * Copyright (c) 2024 Justin's Engineering Services LLC
 * SPDX-License-Identifier: Apache-2.0
*/

#define DT_DRV_COMPAT multiplexer

#define LOG_LEVEL CONFIG_MULTIPLEXER_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(multiplexer);

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#if !DT_NODE_EXISTS(DT_NODELABEL(multiplexer))
#error "Overlay for power output node not properly defined."
#endif

struct multiplexer_cfg {
  struct gpio_dt_spec *gpio_specs;
  size_t select_pins_size;
};

static const struct multiplexer_cfg *dev_cfg(const struct device *dev) {
  return dev->config;
}

int mux_select_out_pin(const struct device *dev, uint8_t output_pin) {}

static int multiplexer_init(const struct device *dev) {
  const struct multiplexer_cfg *cfg = dev_cfg(dev);
  size_t i;

  for (i = 0; i < cfg->select_pins_size; i++) {
    if (!gpio_is_ready_dt(&cfg->gpio_specs[i])) {
      LOG_ERR(
          "Multiplexer GPIO port %s not ready", cfg->gpio_specs[i].port->name
      );
      return -ENODEV;
    }
  }

  return 0;
}

#define MULTIPLEXER_DEVICE(inst)                                      \
                                                                      \
  static struct gpio_dt_spec multiplexer_##inst##_gpio_specs[] = {    \
      DT_FOREACH_PROP_ELEM_SEP(                                       \
          DT_DRV_INST(inst), gpios, GPIO_DT_SPEC_GET_BY_IDX, (, )     \
      )                                                               \
  };                                                                  \
                                                                      \
  static const struct multiplexer_cfg multiplexer_##inst##_cfg = {    \
      .gpio_specs = multiplexer_##inst##_gpio_specs,                  \
      .select_pins_size = ARRAY_SIZE(multiplexer_##inst##_gpio_specs) \
  };                                                                  \
                                                                      \
  DEVICE_DT_INST_DEFINE(                                              \
      inst, multiplexer_init, NULL, NULL, &multiplexer_##inst##_cfg,  \
      POST_KERNEL, CONFIG_MULTIPLEXER_INIT_PRIORITY, NULL                  \
  );

DT_INST_FOREACH_STATUS_OKAY(MULTIPLEXER_DEVICE)
