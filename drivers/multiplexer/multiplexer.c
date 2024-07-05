/*
 * Copyright (c) 2024 Justin's Engineering Services LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT multiplexer

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#define LOG_LEVEL CONFIG_MULTIPLEXER_LOG_LEVEL
LOG_MODULE_REGISTER(multiplexer);

struct multiplexer_cfg {
  struct gpio_dt_spec *gpio_specs;
  size_t select_pins_size;
};

static const struct multiplexer_cfg *dev_cfg(const struct device *dev) {
  return dev->config;
}

int mux_set_active_port(const struct device *dev, uint8_t output_pin) {
  int rc;
  const struct multiplexer_cfg *cfg = dev->config;

  for (size_t i = 0; i < cfg->select_pins_size; i++) {
    if (BIT(cfg->select_pins_size - 1 - i) & output_pin) {
      LOG_DBG("pin %d set HIGH", cfg->gpio_specs[i].pin);
      rc = gpio_pin_set_dt(&cfg->gpio_specs[i], 1);
    } else {
      LOG_DBG("pin %d set LOW", cfg->gpio_specs[i].pin);
      rc = gpio_pin_set_dt(&cfg->gpio_specs[i], 0);
    }
    if (rc) {
      LOG_ERR("Failed to set pin %d on port %s", cfg->gpio_specs[i].pin,
              cfg->gpio_specs[i].port->name);
      return rc;
    }
  }

  return 0;
}

static int multiplexer_init(const struct device *dev) {
  const struct multiplexer_cfg *cfg = dev_cfg(dev);

  for (size_t i = 0; i < cfg->select_pins_size; i++) {
    if (!gpio_is_ready_dt(&cfg->gpio_specs[i])) {
      LOG_ERR("Multiplexer GPIO port %s not ready",
              cfg->gpio_specs[i].port->name);
      return -ENODEV;
    }

    if (gpio_pin_configure_dt(&cfg->gpio_specs[i], GPIO_OUTPUT_INACTIVE)) {
      LOG_ERR("GPIO pin %d (port %s) is not configured as GPIO_OUTPUT",
              cfg->gpio_specs[i].pin, cfg->gpio_specs[i].port->name);
      return -1;
    }
  }

  return 0;
}

#define MULTIPLEXER_DEVICE(inst)                                               \
                                                                               \
  static struct gpio_dt_spec multiplexer_##inst##_gpio_specs[] = {             \
      DT_FOREACH_PROP_ELEM_SEP(DT_DRV_INST(inst), gpios,                       \
                               GPIO_DT_SPEC_GET_BY_IDX, (, ))};                \
                                                                               \
  static const struct multiplexer_cfg multiplexer_##inst##_cfg = {             \
      .gpio_specs = multiplexer_##inst##_gpio_specs,                           \
      .select_pins_size = ARRAY_SIZE(multiplexer_##inst##_gpio_specs)};        \
                                                                               \
  DEVICE_DT_INST_DEFINE(inst, multiplexer_init, NULL, NULL,                    \
                        &multiplexer_##inst##_cfg, POST_KERNEL,                \
                        CONFIG_MULTIPLEXER_INIT_PRIORITY, NULL);

DT_INST_FOREACH_STATUS_OKAY(MULTIPLEXER_DEVICE)
