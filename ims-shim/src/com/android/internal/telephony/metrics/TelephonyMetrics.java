/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.android.internal.telephony.metrics;

import android.telephony.ServiceState;
import android.telephony.ims.ImsCallSession;

/**
 * TelephonyMetrics was removed from the telephony framework in Android 16.
 * The stock MediaTek IMS stack (ImsService, mediatek-telephony-common) still
 * logs through it; keep the methods it calls as no-ops.
 */
public class TelephonyMetrics {
    private static final TelephonyMetrics sInstance = new TelephonyMetrics();

    public static TelephonyMetrics getInstance() {
        return sInstance;
    }

    public void writeOnRilSolicitedResponse(int phoneId, int rilSerial, int rilError,
            int rilRequest, Object ret) {
    }

    public void writeOnRilTimeoutResponse(int phoneId, int rilSerial, int rilRequest) {
    }

    public void writeRilAnswer(int phoneId, int rilSerial) {
    }

    public void writeRilSendSms(int phoneId, int rilSerial, int tech, int format,
            long messageId) {
    }

    public void writeDroppedIncomingMultipartSms(int phoneId, String format,
            int receivedCount, int totalCount) {
    }

    public void writeOnImsCallStart(int phoneId, ImsCallSession session) {
    }

    public void writeServiceStateChanged(int phoneId, ServiceState serviceState) {
    }
}
