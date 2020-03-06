//
// Created by atuser on 2/24/20.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <autogentoo/subsystem/net.h>

NetworkBridge* network_bridge_new(pid_t pid) {
	NetworkBridge* out = malloc(sizeof(NetworkBridge));
	
	out->target = pid;
	out->interface_n = ++interface_n;
	
	asprintf(&out->address_parent, "10.0.0.%d", 2 * out->interface_n);
	asprintf(&out->address_child, "10.0.0.%d", 2 * out->interface_n + 1);
	
	asprintf(&out->name_parent, "veth%d", out->interface_n);
	out->name_child = strdup("eth0");
	
	
	asprintf(&out->init_command, "ip link add name %s type veth peer name %s netns %d", out->name_parent, out->name_child, out->target);
	asprintf(&out->addr_command, "ifconfig %s %s", out->name_parent, out->address_parent);
	asprintf(&out->addr_command_child, "ifconfig %s %s", out->name_child, out->address_child);
	
	return out;
}

int network_bridge_pinit(NetworkBridge* ni) {
	
}