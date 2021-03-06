/**
  args.c

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "shadowvpn.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static const char *help_message =
"usage: shadowvpn -c config_file [-s start/stop/restart] [-v]\n"
"\n"
"  -h, --help            show this help message and exit\n"
"  -s start/stop/restart control shadowvpn process. if omitted, will run\n"
"                        in foreground\n"
"  -c config_file        path to config file\n"
"  -v                    verbose logging\n"
"\n"
"Online help: <https://github.com/clowwindy/ShadowVPN>\n";

static void print_help() __attribute__ ((noreturn));

static void load_default_args(shadowvpn_args_t *args);

static int process_key_value(shadowvpn_args_t *args, const char *key,
                      char *value);

static void print_help() {
  printf("%s", help_message);
  exit(1);
}

static int parse_config_file(shadowvpn_args_t *args, const char *filename) {
  char buf[512];
  char *line;
  FILE *fp;
  size_t len = sizeof(buf);
  int lineno = 0;

  fp = fopen(filename, "rb");
  if (fp == NULL) {
    err("fopen");
    errf("Can't open config file: %s", filename);
    return -1;
  }
  while ((line = fgets(buf, len, fp))) {
    char *sp_pos;
    lineno++;
    sp_pos = strchr(line, '\r');
    if (sp_pos) *sp_pos = '\n';
    sp_pos = strchr(line, '\n');
    if (sp_pos) {
      *sp_pos = 0;
    } else {
      errf("line %d too long in %s", lineno, filename);
      return -1;
    }
    if (*line == 0 || *line == '#')
      continue;
    sp_pos = strchr(line, '=');
    if (!sp_pos) {
      errf("%s:%d: \"=\" is not found in this line: %s", filename, lineno,
           line);
      return -1;
    }
    *sp_pos = 0;
    sp_pos++;
    // line points to key and sp_pos points to value
    if (0 != process_key_value(args, line, sp_pos))
      return 1;
  }
  // check if every required arg is set
  if (!args->mode) {
    errf("mode not set in config file");
    return -1;
  }
  if (!args->server) {
    errf("server not set in config file");
    return -1;
  }
  if (!args->port) {
    errf("port not set in config file");
    return -1;
  }
  if (!args->password) {
    errf("password not set in config file");
    return -1;
  }
  return 0;
}

static int process_key_value(shadowvpn_args_t *args, const char *key,
                      char *value) {
  if (strcmp("password", key) != 0) {
    // set environment variables so that up/down script can
    // make use of these values
    if (-1 == setenv(key, value, 1)) {
      err("setenv");
      return -1;
    }
  }
  if (strcmp("server", key) == 0) {
    args->server = strdup(value);
  } else if (strcmp("port", key) == 0) {
    args->port = atol(value);
  } else if (strcmp("password", key) == 0) {
    args->password = strdup(value);
  } 
  else if (strcmp("net", key) == 0) {
    char* p = strchr(value, '/');
    if (p) *p = 0;
    in_addr_t addr = inet_addr(value);
    if (addr == INADDR_NONE) {
      errf("warning: invalid net IP in config file: %s", value);
    }
    args->netip = ntohl((uint32_t)addr);
  }
  else if (strcmp("mode", key) == 0) {
    if (strcmp("server", value) == 0) {
      args->mode = SHADOWVPN_MODE_SERVER;
    } else if (strcmp("client", value) == 0) {
      args->mode = SHADOWVPN_MODE_CLIENT;
    } else {
      errf("warning: unknown mode in config file: %s", value);
      return -1;
    }
  } else if (strcmp("mtu", key) == 0) {
    long mtu = atol(value);
    // RFC 791
    // in order to wrap packet of length 68, MTU should be > 68 + overhead
    if (mtu < 68 + SHADOWVPN_OVERHEAD_LEN) {
      errf("MTU %ld is too small", mtu);
      return -1;
    }
    if (mtu > MAX_MTU) {
      errf("MTU %ld is too large", mtu);
      return -1;
    }
    args->mtu = mtu;
  } else if (strcmp("intf", key) == 0) {
    args->intf = strdup(value);
  } else if (strcmp("pidfile", key) == 0) {
    args->pid_file = strdup(value);
  } else if (strcmp("logfile", key) == 0) {
    args->log_file = strdup(value);
  } else if (strcmp("up", key) == 0) {
    args->up_script = strdup(value);
  } else if (strcmp("down", key) == 0) {
    args->down_script = strdup(value);
  }
  else {
    errf("warning: config key %s not recognized by shadowvpn, will be "
         "passed to shell scripts anyway", key);
  }
  return 0;
}

static void load_default_args(shadowvpn_args_t *args) {
  args->intf = "tun0";
  args->mtu = 1440;
  args->pid_file = "/var/run/shadowvpn.pid";
  args->log_file = "/var/log/shadowvpn.log";
}

int args_parse(shadowvpn_args_t *args, int argc, char **argv) {
  int ch;
  bzero(args, sizeof(shadowvpn_args_t));
  while ((ch = getopt(argc, argv, "hc:v")) != -1) {
    switch (ch) {
      case 'c':
        args->conf_file = strdup(optarg);
        break;
      case 'v':
        verbose_mode = 1;
        break;
       default:
        print_help();
    }
  }
  if (!args->conf_file)
    print_help();
  load_default_args(args);
  return parse_config_file(args, args->conf_file);
}
