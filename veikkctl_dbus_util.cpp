#include "veikkctl_dbus_util.h"
#include "veikkconfig.h"
#include <unistd.h>
#include <QApplication>

QDBusInterface *VeikkDBusUtil::getInterface()
{
	return new QDBusInterface{VEIKKCTL_DBUS_SERVICE_NAME,
			VEIKKCTL_DBUS_OBJECT_PATH,
			VEIKKCTL_DBUS_INTERFACE_NAME,
			QDBusConnection::systemBus()};
}

bool VeikkDBusUtil::isDaemonRunning()
{
	QDBusConnection conn = QDBusConnection::systemBus();
	QDBusConnectionInterface *dbusConnIntf = conn.interface();

	return dbusConnIntf->isServiceRegistered(VEIKKCTL_DBUS_SERVICE_NAME);
}
