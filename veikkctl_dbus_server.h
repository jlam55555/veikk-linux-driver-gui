/*
 * veikkctl_dbus_server.h -- dbus server object
 *
 * This is an object instantiated in the veikkctl daemon, and it will be exposed
 * as a DBus object at the service VEIKKCTL_DBUS_SERVICE_NAME with object path
 * VEIKKCTL_DBUS_OBJECT_PATH, and with interface VEIKKCTL_DBUS_INTERFACE_NAME.
 */

#ifndef VEIKKCTL_DBUS_SERVER_H
#define VEIKKCTL_DBUS_SERVER_H

#include <QObject>
#include <QApplication>
#include <QtDBus>

#define VEIKKCTL_DBUS_SERVICE_NAME	"com.veikk.VeikkCtl"
#define VEIKKCTL_DBUS_OBJECT_PATH	"/com/veikk/VeikkCtl"
#define VEIKKCTL_DBUS_INTERFACE_NAME	"com.veikk.VeikkCtl"

class VeikkDBusServer : public QObject
{
	Q_OBJECT

	QCoreApplication &app;

public:
	VeikkDBusServer(QCoreApplication &app);

public slots:
	void reload();
	void stop();
};

#endif // VEIKKCTL_DBUS_SERVER_H
