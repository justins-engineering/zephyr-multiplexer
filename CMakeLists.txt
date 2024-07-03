# Copyright (c) 2024 Justin's Engineering Services LLC
# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(include)
add_subdirectory_ifdef(CONFIG_MULTIPLEXER drivers/multiplexer)
