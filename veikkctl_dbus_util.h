/*
 * veikkctl_dbus_util.h -- manage dbus server
 *
 * This class provides the methods to start the veikkctl dbus daemon, and to
 * send controls to it.
 *
 * Since this is just a collection of methods, it does not subclass QObject.
 */

#ifndef VEIKKCTL_DBUS_UTIL_H
#define VEIKKCTL_DBUS_UTIL_H

#include "veikkctl_dbus_server.h"

class VeikkDBusUtil
{
	static QDBusInterface *getInterface();
	static bool isDaemonRunning();

public:
	static bool startDaemon();
	static bool reloadDaemon();
	static bool stopDaemon();

	// a collection of utility fns doesn't need a constructor
	VeikkDBusUtil() = delete;
};

#endif // VEIKKCTL_DBUS_UTIL_H
