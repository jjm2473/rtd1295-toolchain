#
# This software is licensed under the CC0-1.0 license.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=rtd1295-init
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_LICENSE:=CC0-1.0

PKG_MAINTAINER:=jjm2473 <jjm2473@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/rtd1295-init
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=rtd1295 openwrt+android double os init
	MAINTAINER:=jjm2473 <jjm2473@gmail.com>
endef

define Package/rtd1295-init/description
	This package contains init program for rtd1295 to boot to openwrt+android double os.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/rtd1295-init/install
	$(INSTALL_DIR) $(1)/tmp
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/init $(1)/tmp
endef

$(eval $(call BuildPackage,rtd1295-init))
