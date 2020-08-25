#include "veikkctl_dbus_server.h"
#include "veikkctl_dbus_util.h"

VeikkDBusServer::VeikkDBusServer(QCoreApplication &app) : app{app} { }

void VeikkDBusServer::reload()
{
	qInfo() << "In reload()";

	// reload mapping services
}

void VeikkDBusServer::stop()
{
	// can perform other cleanup actions in mapping daemons here

	app.quit();
}
