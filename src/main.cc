/**
  main.c

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "shadowvpn.h"

static vpn_ctx_t vpn_ctx;

static void sig_handler(int signo) {
    vpn_stop(&vpn_ctx);
}

int main(int argc, char **argv) {
  shadowvpn_args_t args;
  // parse args
  if (0 != args_parse(&args, argc, argv)) {
    errf("error when parsing args");
    return EXIT_FAILURE;
  }

  if (0 != crypto_init()) {
    errf("shadowvpn_crypto_init");
    return EXIT_FAILURE;
  }

  if (0 !=crypto_set_password(args.password, strlen(args.password))) {
    errf("can not set password");
    return EXIT_FAILURE;
  }

  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);

  if (-1 == vpn_ctx_init(&vpn_ctx, &args)) {
    return EXIT_FAILURE;
  }
  return vpn_run(&vpn_ctx);
}
