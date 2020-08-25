#include "veikkctl_daemon.h"

// register dbus service and object
int VeikkCtlDaemon::registerDBusServerObject(VeikkDBusServer &server)
{
	QDBusConnection conn = QDBusConnection::systemBus();

	if (!conn.registerService(VEIKKCTL_DBUS_SERVICE_NAME)) {
		qWarning() << "error registering dbus service";
		return -1;
	}

	if (!conn.registerObject(VEIKKCTL_DBUS_OBJECT_PATH,
			VEIKKCTL_DBUS_INTERFACE_NAME, &server,
			QDBusConnection::ExportAllSlots)) {
		qWarning() << "error registering dbus object";
		return -1;
	}

	return 0;
}


int VeikkCtlDaemon::startMappingThreads()
{
	qInfo() << "In start mapping threads";
	return 0;
}
