#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixups,
    lib_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)

namespace_imports = [
    'device/xiaomi/camellia',
    'hardware/mediatek',
    'hardware/mediatek/libmtkperf_client',
    'hardware/xiaomi',
]

lib_fixups: lib_fixups_user_type = {
    **lib_fixups,
}


blob_fixups: blob_fixups_user_type = {
    'system_ext/lib64/libsink.so': blob_fixup()
        .add_needed('libshim_sink.so'),
    'system_ext/lib64/libsource.so': blob_fixup()
        .patchelf_version('0_18')
        .add_needed('libui_shim.so'),
    ('vendor/bin/hw/android.hardware.gnss-service.mediatek',
     'vendor/lib64/hw/android.hardware.gnss-impl-mediatek.so'): blob_fixup()
        .replace_needed('android.hardware.gnss-V1-ndk_platform.so', 'android.hardware.gnss-V1-ndk.so'),
    'vendor/bin/hw/android.hardware.media.c2@1.2-mediatek': blob_fixup()
        .add_needed('libstagefright_foundation-v33.so'),
    'vendor/bin/hw/android.hardware.memtrack-service.mediatek': blob_fixup()
        .replace_needed('android.hardware.memtrack-V1-ndk_platform.so', 'android.hardware.memtrack-V1-ndk.so'),
    ('vendor/bin/hw/android.hardware.neuralnetworks@1.3-service-mtk-neuron',
     'vendor/lib/libnvram.so',
     'vendor/lib64/libnvram.so',
     'vendor/lib64/libsysenv.so'): blob_fixup()
        .patchelf_version('0_18')
        .add_needed('libbase_shim.so'),
    'vendor/bin/hw/mtkfusionrild': blob_fixup()
        .patchelf_version('0_18')
        .add_needed('libutils-v32.so'),
    ('vendor/bin/mnld',
     'vendor/lib/libaalservice.so',
     'vendor/lib64/libaalservice.so',
     'vendor/lib64/libcam.utils.sensorprovider.so'): blob_fixup()
        .add_needed('libshim_sensors.so'),
    'vendor/etc/init/android.hardware.neuralnetworks@1.3-service-mtk-neuron.rc': blob_fixup()
        .regex_replace('start', 'enable'),
    'vendor/etc/vintf/manifest/manifest_media_c2_V1_2_default.xml': blob_fixup()
        .regex_replace('1.1', '1.2'),
    ('vendor/lib/hw/vendor.mediatek.hardware.pq@2.13-impl.so',
     'vendor/lib64/hw/vendor.mediatek.hardware.pq@2.13-impl.so',
     'vendor/lib64/libmtkcam_stdutils.so'): blob_fixup()
        .replace_needed('libutils.so', 'libutils-v32.so'),
    'vendor/lib64/hw/android.hardware.camera.provider@2.6-impl-mediatek.so': blob_fixup()
        .patchelf_version('0_18')
        .replace_needed('libutils.so', 'libutils-v32.so'),
}  # fmt: skip

module = ExtractUtilsModule(
    'camellia',
    'xiaomi',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
)

if __name__ == '__main__':
    utils = ExtractUtils.device(module)
    utils.run()
