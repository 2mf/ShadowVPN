/**
  args.h

  Copyright (C) 2015 clowwindy

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#define MAX_MTU 9000

typedef enum {
  SHADOWVPN_MODE_SERVER = 1,
  SHADOWVPN_MODE_CLIENT = 2
} shadowvpn_mode;

typedef struct {
  shadowvpn_mode mode;
  const char *conf_file;
  const char *pid_file;
  const char *log_file;
  const char *intf;
  const char *password;
  const char *server;
  uint16_t port;
  uint16_t mtu;

  // the ip of the "net" configuration
  // in host order
  uint32_t netip;
  char (*user_tokens)[8];
  size_t user_tokens_len;

  const char *up_script;
  const char *down_script;
} shadowvpn_args_t;

int args_parse(shadowvpn_args_t *args, int argc, char **argv);

#endif
