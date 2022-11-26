ifneq ($(CROSS_COMPILE),)
CROSS-COMPILE:=$(CROSS_COMPILE)
endif

ifeq ($(CC),cc)
CC:=$(CROSS-COMPILE)gcc
endif

LD:=$(CROSS-COMPILE)ld
STRIP:=$(CROSS-COMPILE)strip

QL_CM_SRC=QmiWwanCM.c GobiNetCM.c main.c MPQMUX.c QMIThread.c util.c qmap_bridge_mode.c mbim-cm.c device.c
QL_CM_SRC+=atc.c atchannel.c at_tok.c
#QL_CM_SRC+=qrtr.c rmnetctl.c
ifeq (1,1)
QL_CM_DHCP=udhcpc.c
else
LIBMNL=libmnl/ifutils.c libmnl/attr.c libmnl/callback.c libmnl/nlmsg.c libmnl/socket.c
DHCP=libmnl/dhcp/dhcpclient.c libmnl/dhcp/dhcpmsg.c libmnl/dhcp/packet.c
QL_CM_DHCP=udhcpc_netlink.c
QL_CM_DHCP+=${LIBMNL}
endif

CFLAGS += -Wall -Werror -Wno-format-truncation -O2 #-s
LDFLAGS += -lpthread -ldl -lrt

all: release

release: clean qmi-proxy mbim-proxy
	$(CC) ${CFLAGS} ${QL_CM_SRC} ${QL_CM_DHCP} -o quectel-cm ${LDFLAGS}
	$(STRIP) quectel-cm
	$(STRIP) quectel-qmi-proxy
	$(STRIP) quectel-mbim-proxy

debug: clean
	$(CC) ${CFLAGS} -g -DCM_DEBUG ${QL_CM_SRC} ${QL_CM_DHCP} -o quectel-cm ${LDFLAGS}

qmi-proxy:
	$(CC) ${CFLAGS} quectel-qmi-proxy.c -o quectel-qmi-proxy ${LDFLAGS} 

mbim-proxy:
	$(CC) ${CFLAGS} quectel-mbim-proxy.c -o quectel-mbim-proxy ${LDFLAGS} 

clean:
	rm -rf *.o libmnl/*.o quectel-cm quectel-qmi-proxy quectel-mbim-proxy
