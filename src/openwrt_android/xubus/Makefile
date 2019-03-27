#
# This software is licensed under the CC0-1.0 license.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=rtd1295-xubus
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_LICENSE:=CC0-1.0

PKG_MAINTAINER:=jjm2473 <jjm2473@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/rtd1295-xubus
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=rtd1295 openwrt+android double os global ubus
	MAINTAINER:=jjm2473 <jjm2473@gmail.com>
endef

define Package/rtd1295-xubus/description
	This package contains ubus proxy program for rtd1295 to provide ubus in openwrt+android double os.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/rtd1295-xubus/install
	$(INSTALL_DIR) $(1)/tmp
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xubus $(1)/tmp
endef

$(eval $(call BuildPackage,rtd1295-xubus))
