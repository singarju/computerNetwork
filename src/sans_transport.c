#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include "include/sans.h"
#include "include/rudp.h"
#include "include/socket_map.h"

int sans_send_pkt(int socket, const char* buf, int len) {
  struct sockaddr *address;
  socklen_t addr_len;
  int seqnum;
  for (size_t i = 0; i <  MAX_SOCKET_COUNT; i++) {
    if (socket_map[i].socket_id == socket) {
        address = socket_map[i].address;
        addr_len = socket_map[i].addr_len;
        seqnum = socket_map[i].seqnum;
        socket_map[i].seqnum++;
        break;
    }
  }
  rudp_packet_t *packet = malloc(sizeof(rudp_packet_t) + len);
  packet->type = DAT;
  packet->seqnum = seqnum;
  memcpy(packet->payload, buf, len);
  int bytes_sent = sendto(socket, packet, sizeof(rudp_packet_t) + len, 0, address, addr_len);
  return bytes_sent;
}

int sans_recv_pkt(int socket, char* buf, int len) {
  struct sockaddr *address;
  socklen_t addr_len;
  for (size_t i = 0; i <  MAX_SOCKET_COUNT; i++) {
    if (socket_map[i].socket_id == socket) {
        address = socket_map[i].address;
        addr_len = socket_map[i].addr_len;
        break;
    }
  }
  rudp_packet_t response;
  int bytes_received = recvfrom(socket, &response, sizeof(response), 0, address, &addr_len);
  int payload_len = bytes_received - sizeof(rudp_packet_t);
  memcpy(buf, response.payload, payload_len);
  buf[payload_len] = '\0';
  return payload_len;
}
