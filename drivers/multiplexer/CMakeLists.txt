# Copyright (c) 2024 Justin's Engineering Services LLC
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_MULTIPLEXER multiplexer.c)
