#include "veikkctl_dbus_server.h"

VeikkDBusServer::VeikkDBusServer(QApplication &app) : app{app} {}

void VeikkDBusServer::reload()
{
	qInfo() << "In reload()";

	// reload mapping services
}

void VeikkDBusServer::stop()
{
	qInfo() << "In stop()";

	// quit veikk daemon
	app.quit();
}
