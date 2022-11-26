//https://github.com/andersson/qrtr
/******************************************************************************
  @file    QrtrCM.c
  @brief   GobiNet driver.

  DESCRIPTION
  Connectivity Management Tool for USB network adapter of Quectel wireless cellular modules.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 - 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <ctype.h>
#include "QMIThread.h"
#include <linux/qrtr.h>

#ifndef AF_QIPCRTR
#define AF_QIPCRTR 42
#endif

#ifndef QRTR_NODE_BCAST

#define QRTR_NODE_BCAST 0xffffffffu
#define QRTR_PORT_CTRL  0xfffffffeu

enum qrtr_pkt_type {
        QRTR_TYPE_DATA          = 1,
        QRTR_TYPE_HELLO         = 2,
        QRTR_TYPE_BYE           = 3,
        QRTR_TYPE_NEW_SERVER    = 4,
        QRTR_TYPE_DEL_SERVER    = 5,
        QRTR_TYPE_DEL_CLIENT    = 6,
        QRTR_TYPE_RESUME_TX     = 7,
        QRTR_TYPE_EXIT          = 8,
        QRTR_TYPE_PING          = 9,
        QRTR_TYPE_NEW_LOOKUP    = 10,
        QRTR_TYPE_DEL_LOOKUP    = 11,
};

struct qrtr_ctrl_pkt {
        uint32_t cmd;

        union {
                struct {
                        uint32_t service;
                        uint32_t instance;
                        uint32_t node;
                        uint32_t port;
                } server;

                struct {
                        uint32_t node;
                        uint32_t port;
                } client;
        };
} __attribute__((packed));
#endif

typedef struct {
    uint32_t service;
    uint32_t version;
    uint32_t instance;
    uint32_t node;
    uint32_t port;
} QrtrService;

#define QRTR_MAX QMUX_TYPE_WDS_ADMIN
static QrtrService service_list[QRTR_MAX];

static int qmiclientId[QMUX_TYPE_WDS_ADMIN + 1];

static int qrtr_socket(void)
{
    struct sockaddr_qrtr sq;
    socklen_t sl = sizeof(sq);
    int sock;
    int rc;

    sock = socket(AF_QIPCRTR, SOCK_DGRAM, 0);
    if (sock < 0) {
        dbg_time("socket errno: %d (%s)\n", errno, strerror(errno));
        return -1;
    }

    rc = getsockname(sock, (void *)&sq, &sl);
    if (rc || sq.sq_family != AF_QIPCRTR || sl != sizeof(sq)) {
        dbg_time("getsockname: %d (%s)\n", errno, strerror(errno));
        close(sock);
        return -1;
    }

    return sock;
}

static int qrtr_sendto(int sock, uint32_t node, uint32_t port, const void *data, unsigned int sz)
{
    struct sockaddr_qrtr sq = {};
    int rc;

    sq.sq_family = AF_QIPCRTR;
    sq.sq_node = node;
    sq.sq_port = port;

    rc = sendto(sock, data, sz, 0, (void *)&sq, sizeof(sq));
    if (rc < 0) {
        dbg_time("sendto errno: %d (%s)\n", errno, strerror(errno));
        return -1;
    }

    return 0;
}

static int get_client(UCHAR QMIType) {
    int ClientId;
    QrtrService *s = &service_list[QMIType];

    if (!s ->service) {
        dbg_time("%s service: %d for QMIType: %d", __func__, s ->service, QMIType);
        return -ENODEV;
    }

    ClientId = qrtr_socket();
    if (ClientId == -1) {
        return -1;
    }

    switch (QMIType) {
        case QMUX_TYPE_WDS: dbg_time("Get clientWDS = %d", ClientId); break;
        case QMUX_TYPE_DMS: dbg_time("Get clientDMS = %d", ClientId); break;
        case QMUX_TYPE_NAS: dbg_time("Get clientNAS = %d", ClientId); break;
        case QMUX_TYPE_QOS: dbg_time("Get clientQOS = %d", ClientId); break;
        case QMUX_TYPE_WMS: dbg_time("Get clientWMS = %d", ClientId); break;
        case QMUX_TYPE_PDS: dbg_time("Get clientPDS = %d", ClientId); break;
        case QMUX_TYPE_UIM: dbg_time("Get clientUIM = %d", ClientId); break;
        case QMUX_TYPE_WDS_ADMIN: dbg_time("Get clientWDA = %d", ClientId);
        break;
        default: break;
    }

    return ClientId;
}

static int qmi_send(PQCQMIMSG pRequest) {
    uint8_t QMIType = pRequest->QMIHdr.QMIType;
    int sock;
    QrtrService *s = &service_list[QMIType == QMUX_TYPE_WDS_IPV6 ? QMUX_TYPE_WDS: QMIType];
    sock = qmiclientId[QMIType];

    pRequest->QMIHdr.ClientId = 1;
    if (!s ->service || !sock) {
        dbg_time("%s service: %d, sock: %d for QMIType: %d", __func__, s ->service, sock, QMIType);
        return -ENODEV;
    }

    return qrtr_sendto(sock, s->node, s->port, &pRequest->MUXMsg,
        le16_to_cpu(pRequest->QMIHdr.Length) + 1 - sizeof(QCQMI_HDR));
}

static int qmi_deinit(void) {
    unsigned int i;
    for (i = 0; i < sizeof(qmiclientId)/sizeof(qmiclientId[0]); i++)
    {
        if (qmiclientId[i] != 0)
        {
            close(qmiclientId[i]);
            qmiclientId[i] = 0;
        }
    }

    return 0;
}

static void handle_crtrl_pkt(int sock) {
    struct qrtr_ctrl_pkt pkt;
    struct sockaddr_qrtr sq;
    socklen_t sl = sizeof(sq);
    uint32_t type;
    int rc;

    rc = recvfrom(sock, &pkt, sizeof(pkt), 0, (void *)&sq, &sl);
    if (rc < 0)
        return;

    type = le32toh(pkt.cmd);
    //dbg_time("type %u, node %u, sq.port %x, len: %d", type, sq.sq_node, sq.sq_port, rc);

    if (sq.sq_port != QRTR_PORT_CTRL)
        return;

    if (QRTR_TYPE_NEW_SERVER == type || QRTR_TYPE_DEL_SERVER == type) {
        QrtrService s;

        s.service = le32toh(pkt.server.service);
        s.version = le32toh(pkt.server.instance) & 0xff;
        s.instance = le32toh(pkt.server.instance) >> 8;
        s.node = le32toh(pkt.server.node);
        s.port = le32toh(pkt.server.port);

        //dbg_time ("[qrtr] %s  server on %u:%u -> service %u, version %u, instance %u",
        //            QRTR_TYPE_NEW_SERVER == type ? "add" : "remove",
        //             s.node, s.port, s.service, s.version, s.instance);

        if (QRTR_TYPE_NEW_SERVER == type) {
            if (s.service == QMUX_TYPE_WDS
                || s.service == QMUX_TYPE_NAS
                || s.service == QMUX_TYPE_UIM
                || s.service == QMUX_TYPE_DMS
                || s.service == QMUX_TYPE_WDS_ADMIN)
            {
                service_list[s.service] = s;
                if (qmiclientId[s.service] == 0) {
                     qmiclientId[s.service] = get_client(s.service);
                     if (s.service == QMUX_TYPE_WDS) {
                        qmiclientId[QMUX_TYPE_WDS_IPV6] = get_client(QMUX_TYPE_WDS);
                     }
                }
            }
            else if (s.service == 0) {
                qmidevice_send_event_to_main(RIL_INDICATE_DEVICE_CONNECTED);
            }
        }
        else if (QRTR_TYPE_DEL_SERVER == type) {
            if (s.service < QRTR_MAX) {
                memset(&service_list[s.service], 0, sizeof(QrtrService));
            }
        }
    }
}

static void * qmi_read(void *pData) {
    PROFILE_T *profile = (PROFILE_T *)pData;
    struct qrtr_ctrl_pkt pkt;
    struct sockaddr_qrtr sq;
    socklen_t sl = sizeof(sq);
    int sock;
    int rc;
    int wait_for_request_quit = 0;   

    sock = qrtr_socket();
    if (sock == -1)
        goto _quit;

    memset(&pkt, 0, sizeof(pkt));
    pkt.cmd = htole32(QRTR_TYPE_NEW_LOOKUP);

    getsockname(sock, (void *)&sq, &sl);
    rc = qrtr_sendto(sock, sq.sq_node, QRTR_PORT_CTRL, &pkt, sizeof(pkt));
    if (rc == -1)
        goto _quit;

    while (1) {
        struct pollfd pollfds[16] = {{qmidevice_control_fd[1], POLLIN, 0}, {sock, POLLIN, 0}};
        int ne, ret, nevents = 2;
        unsigned int i;

        for (i = 0; i < sizeof(qmiclientId)/sizeof(qmiclientId[0]); i++)
        {
            if (qmiclientId[i] != 0)
            {
                pollfds[nevents].fd = qmiclientId[i];
                pollfds[nevents].events = POLLIN;
                pollfds[nevents].revents = 0;
                nevents++;
            }
        }

        do {
            ret = poll(pollfds, nevents, wait_for_request_quit ? 1000 : -1);
         } while ((ret < 0) && (errno == EINTR));

	if (ret == 0 && wait_for_request_quit) {
            QmiThreadRecvQMI(NULL); //main thread may pending on QmiThreadSendQMI()
            continue;
	}

        if (ret <= 0) {
            dbg_time("%s poll=%d, errno: %d (%s)", __func__, ret, errno, strerror(errno));
            break;
        }

        for (ne = 0; ne < nevents; ne++) {
            int fd = pollfds[ne].fd;
            short revents = pollfds[ne].revents;

            if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
                dbg_time("%s poll err/hup/inval", __func__);
                dbg_time("epoll fd = %d, events = 0x%04x", fd, revents);
                if (fd == qmidevice_control_fd[1]) {
                } else {
                }
                if (revents & (POLLERR | POLLHUP | POLLNVAL))
                    goto _quit;
            }

            if ((revents & POLLIN) == 0)
                continue;

            if (fd == qmidevice_control_fd[1]) {
                int triger_event;
                if (read(fd, &triger_event, sizeof(triger_event)) == sizeof(triger_event)) {
                    //DBG("triger_event = 0x%x", triger_event);
                    switch (triger_event) {
                        case RIL_REQUEST_QUIT:
                            goto _quit;
                        break;
                        case SIG_EVENT_STOP:
                            wait_for_request_quit = 1;   
                        break;
                        default:
                        break;
                    }
                }
            }
            else  if (fd == sock) {
                handle_crtrl_pkt(sock);
                if ( profile->wda_client != qmiclientId[QMUX_TYPE_WDS_ADMIN])
                    profile->wda_client = qmiclientId[QMUX_TYPE_WDS_ADMIN];
            }
            else
            {
                PQCQMIMSG pResponse = (PQCQMIMSG)cm_recv_buf;

                sl = sizeof(sq);
                rc = recvfrom(fd, &pResponse->MUXMsg, sizeof(cm_recv_buf) - sizeof(QCQMI_HDR), 0, (void *)&sq, &sl);
                //dbg_time(" fd %d, node %u, sq.port %x, len: %d", fd, sq.sq_node, sq.sq_port, rc);
                if (rc <= 0)
                {
                    dbg_time("%s read=%d errno: %d (%s)",  __func__, rc, errno, strerror(errno));
                    break;
                }

                for (i = 0; i < sizeof(qmiclientId)/sizeof(qmiclientId[0]); i++)
                {
                    if (qmiclientId[i] == fd)
                    {
                        pResponse->QMIHdr.QMIType = i;

                        if (service_list[i].node != sq.sq_node || service_list[i].port != sq.sq_port) {
                            continue;
                        }
                    }
                }

                pResponse->QMIHdr.IFType = USB_CTL_MSG_TYPE_QMI;
                pResponse->QMIHdr.Length = cpu_to_le16(rc + sizeof(QCQMI_HDR)  - 1);
                pResponse->QMIHdr.CtlFlags = 0x00;
                pResponse->QMIHdr.ClientId = 1;

                QmiThreadRecvQMI(pResponse);
            }
        }
    }

_quit:
    qmi_deinit();
    qmidevice_send_event_to_main(RIL_INDICATE_DEVICE_DISCONNECTED);
    QmiThreadRecvQMI(NULL); //main thread may pending on QmiThreadSendQMI()
    dbg_time("%s exit", __func__);
    pthread_exit(NULL);
    return NULL;
}

const struct qmi_device_ops qrtr_qmidev_ops = {
	.deinit = qmi_deinit,
	.send = qmi_send,
	.read = qmi_read,
};

