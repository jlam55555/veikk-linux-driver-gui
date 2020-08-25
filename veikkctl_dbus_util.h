/*
 * veikkctl_dbus_util.h -- helper functions for dbus connection
 *
 * Since this is just a collection of methods, it does not subclass QObject.
 */

#ifndef VEIKKCTL_DBUS_UTIL_H
#define VEIKKCTL_DBUS_UTIL_H

#include "veikkctl_dbus_server.h"

class VeikkDBusUtil
{
public:
	// get veikkctl dbus interface
	static QDBusInterface *getInterface();

	// check if dbus server (and veikkctl daemon) is running
	static bool isDaemonRunning();

	// a collection of utility fns doesn't need a constructor
	VeikkDBusUtil() = delete;
};

#endif // VEIKKCTL_DBUS_UTIL_H
