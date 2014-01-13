#ifndef PARSER_H
#define PARSER_H

#include "protocol.h"


bool command_valid(const uint8_t& type);

bool parse_packet(uint8_t* payload, uint8_t len, Message* msg);

bool check_crc(uint8_t* payload, uint8_t len);
#endif