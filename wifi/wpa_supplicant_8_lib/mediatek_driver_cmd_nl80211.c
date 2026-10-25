/*
 * SPDX-FileCopyrightText: 2026 bjgo78
 * SPDX-License-Identifier: Apache-2.0
 *
 * wpa_supplicant private driver command library for MediaTek gen4m
 * (CONNAC) Wi-Fi, e.g. MT6833 with the stock wlan_drv_gen4m.ko.
 *
 * Android sends "driver commands" (COUNTRY, SETSUSPENDMODE, BTCOEX*,
 * RXFILTER*, MIRACAST) through wpa_supplicant. gen4m receives
 * them on SIOCDEVPRIVATE + 1 in priv_support_driver_cmd(), which expects
 * struct priv_driver_cmd_s with an inline 512 byte buffer (not the AOSP /
 * Broadcom layout with a char pointer).
 */

#include "includes.h"

#include <net/if.h>
#include <sys/ioctl.h>

#include "common.h"
#include "driver_nl80211.h"
#include "linux_ioctl.h"

#define MTK_PRIV_CMD_SIZE 512

/* Must match struct priv_driver_cmd_s in gen4m os/linux/gl_wext_priv.c
 * (driver built without CFG_ANDROID_AOSP_PRIV_CMD). */
struct mtk_priv_driver_cmd {
	char buf[MTK_PRIV_CMD_SIZE];
	int used_len;
	int total_len;
};

static int mtk_send_priv_cmd(struct i802_bss *bss, const char *cmd,
			     char *reply, size_t reply_len)
{
	struct wpa_driver_nl80211_data *drv = bss->drv;
	struct mtk_priv_driver_cmd priv_cmd;
	struct ifreq ifr;
	int ret;

	if (os_strlen(cmd) >= sizeof(priv_cmd.buf)) {
		wpa_printf(MSG_ERROR, "mtk_driver_cmd: command too long: %s",
			   cmd);
		return -1;
	}

	os_memset(&priv_cmd, 0, sizeof(priv_cmd));
	os_strlcpy(priv_cmd.buf, cmd, sizeof(priv_cmd.buf));
	priv_cmd.used_len = os_strlen(priv_cmd.buf) + 1;
	priv_cmd.total_len = sizeof(priv_cmd.buf);

	os_memset(&ifr, 0, sizeof(ifr));
	os_strlcpy(ifr.ifr_name, bss->ifname, IFNAMSIZ);
	ifr.ifr_data = (void *) &priv_cmd;

	ret = ioctl(drv->global->ioctl_sock, SIOCDEVPRIVATE + 1, &ifr);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "mtk_driver_cmd: '%s' on %s failed: %s",
			   cmd, bss->ifname, strerror(errno));
		return ret;
	}

	priv_cmd.buf[sizeof(priv_cmd.buf) - 1] = '\0';
	wpa_printf(MSG_DEBUG, "mtk_driver_cmd: '%s' on %s ok, reply '%s'",
		   cmd, bss->ifname, priv_cmd.buf);

	if (reply && reply_len > 0)
		os_strlcpy(reply, priv_cmd.buf, reply_len);

	return 0;
}

int wpa_driver_nl80211_driver_cmd(void *priv, char *cmd, char *buf,
				  size_t buf_len)
{
	struct i802_bss *bss = priv;
	struct wpa_driver_nl80211_data *drv = bss->drv;
	int ret;

	/* gen4m has no MACADDR command; answer it from the netdev. */
	if (os_strcasecmp(cmd, "MACADDR") == 0) {
		u8 macaddr[ETH_ALEN] = {};

		ret = linux_get_ifhwaddr(drv->global->ioctl_sock, bss->ifname,
					 macaddr);
		if (ret < 0)
			return ret;
		return os_snprintf(buf, buf_len, "Macaddr = " MACSTR "\n",
				   MAC2STR(macaddr));
	}

	ret = mtk_send_priv_cmd(bss, cmd, buf, buf_len);
	if (ret < 0)
		return ret;

	if (os_strncasecmp(cmd, "COUNTRY ", 8) == 0) {
		union wpa_event_data event;

		os_memset(&event, 0, sizeof(event));
		event.channel_list_changed.initiator = REGDOM_SET_BY_USER;
		event.channel_list_changed.type = REGDOM_TYPE_COUNTRY;
		os_strlcpy(event.channel_list_changed.alpha2, cmd + 8,
			   sizeof(event.channel_list_changed.alpha2));
		wpa_supplicant_event(drv->ctx, EVENT_CHANNEL_LIST_CHANGED,
				     &event);
	}

	/* Callers (AIDL, ctrl_iface) treat any non-zero value as failure. */
	return 0;
}

/*
 * P2P hooks: gen4m's P2P_SET_NOA/P2P_SET_PS use MediaTek specific arguments
 * (role index, CT window), not the Broadcom style ones wpa_supplicant passes
 * here. Keep the same results as the AOSP stub so P2P behaves as before.
 */
int wpa_driver_set_p2p_noa(void *priv, u8 count, int start, int duration)
{
	return 0;
}

int wpa_driver_get_p2p_noa(void *priv, u8 *buf, size_t len)
{
	return 0;
}

int wpa_driver_set_p2p_ps(void *priv, int legacy_ps, int opp_ps, int ctwindow)
{
	return -1;
}

int wpa_driver_set_ap_wps_p2p_ie(void *priv, const struct wpabuf *beacon,
				 const struct wpabuf *proberesp,
				 const struct wpabuf *assocresp)
{
	return 0;
}
