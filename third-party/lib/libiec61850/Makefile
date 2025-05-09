
PKG_NAME := libiec61850
PKG_VER  := 1.5.1
PKG_SOURCES := https://github.com/mz-automation/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 4f4a2c5327ffc5c4d28c8c804c654b2e

PKG_PATCHES := makefile.patch
PKG_PATCHES += mms_client_files.patch
PKG_PATCHES += sv_subscriber.patch
PKG_PATCHES += socket_linux.patch
PKG_PATCHES += ethernet_linux.patch
PKG_PATCHES += file_provider_linux.patch
PKG_PATCHES += serial_port_linux.patch
PKG_PATCHES += conversions.patch
PKG_PATCHES += iec61850_common.patch
PKG_PATCHES += platform_endian.patch

export EMBOX_CFLAGS
export EMBOX_LDFLAGS
export SRC_DIR
export EMBOX_GCC
export EMBOX_ARCH

$(info AUTOCONF_ARCH: $(AUTOCONF_ARCH))
$(info EMBOX_CFLAGS: $(EMBOX_CFLAGS))
$(info EMBOX_LDLAGS: $(EMBOX_LDFLAGS))
$(info EMBOX_ARCH:   $(EMBOX_ARCH))

CONFIG_NAME := $(call option_get,STRING,config)
TLS_ENABLE  := $(call option_get,BOOLEAN,tls_enable)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full

$(BUILD) :
	cp config/$(CONFIG_NAME).h $(PKG_SOURCE_DIR)/config/stack_config.h
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) WITH_MBEDTLS=TLS_ENABLE TARGET=EMBOX MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) TARGET=EMBOX INSTALL_PREFIX='$(PKG_INSTALL_DIR)' install; \
	)
	touch $@
