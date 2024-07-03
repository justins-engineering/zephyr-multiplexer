/*
 * Copyright (c) 2024 Justin's Engineering Services LLC
 * SPDX-License-Identifier: Apache-2.0
*/

#ifndef ZEPHYR_DRIVERS_MULTIPLEXER_H_
#define ZEPHYR_DRIVERS_MULTIPLEXER_H_

#include <zephyr/kernel.h>
#include <zephyr/types.h>

int mux_select_out_pin(const struct device *dev, uint8_t output_pin);

#endif // ZEPHYR_DRIVERS_MULTIPLEXER_H_
