/*
 * veikkctl_daemon.h -- daemon code
 *
 * Static class is used only for namespacing
 */

#ifndef VEIKKCTL_DAEMON_H
#define VEIKKCTL_DAEMON_H

#include "veikkctl_dbus_server.h"

class VeikkCtlDaemon
{
public:
	static int registerDBusServerObject(VeikkDBusServer &server);
	static int startMappingThreads();

	// TODO:
	// stopMappingThreads();
	// getVeikkDevices();

	VeikkCtlDaemon() = delete;
};

#endif // VEIKKCTL_DAEMON_H
