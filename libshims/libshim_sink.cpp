/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * libsink.so needs the AudioTrack constructor that ends with a
 * std::string (codecProvenance). Android 17's libaudioclient provides it
 * itself, so there is nothing left to shim. The library stays (empty)
 * because libsink.so carries a DT_NEEDED entry for it (extract-files.py).
 */
