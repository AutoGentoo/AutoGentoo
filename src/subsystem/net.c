//
// Created by atuser on 2/24/20.
//

#include <autogentoo/subsystem/net.h>

NetworkInterface* network_interface_new(pid_t pid) {
	NetworkInterface* out = malloc(sizeof(NetworkInterface));
	
	out->target = pid;
	out->interface_n = interface_n++;
	
	asprintf(&out->address_parent, "10.0.0.%d", 2 * (out->interface_n + 1));
	asprintf(&out->address_child, "10.0.0.%d", 2 * (out->interface_n + 1) + 1);
	
	asprintf(&out->name_parent, "veth%d", out->interface_n * 2);
	asprintf(&out->name_child, "veth%d", out->interface_n * 2 + 1);
	
	asprintf(&out->init_command, "ip link add name %s type veth peer name %s netns %d", out->name_parent, out->name_child, out->target);
	asprintf(&out->addr_command, "ifconfig %s %s", out->name_parent, out->address_parent);
	asprintf(&out->addr_command_child, "ifconfig %s %s", out->name_child, out->address_child);
	
	return out;
}