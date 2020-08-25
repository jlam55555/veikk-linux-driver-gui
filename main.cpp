#include "veikkconfig.h"
#include "veikkctl_dbus_util.h"
#include "veikkctl_daemon.h"
#include <QApplication>
#include <QDebug>

// starts up veikk dbus daemon and mapping threads
static int startVeikkCtlDaemon(QCoreApplication &app)
{
	int err;
	VeikkDBusServer server{app};

	if ((err = VeikkCtlDaemon::registerDBusServerObject(server)))
		return err;

	if ((err = VeikkCtlDaemon::startMappingThreads()))
		return err;

	return app.exec();
}

// sends a message to the daemon to quit
static int quitVeikkCtlDaemon()
{
	QDBusInterface *intf = VeikkDBusUtil::getInterface();

	if (!VeikkDBusUtil::isDaemonRunning()) {
		qWarning() << "veikkctl daemon not running. nothing to do";
		return -1;
	}

	intf->call("stop");
	delete intf;
	return 0;
}

// sends a message to daemon to reload mapping threads
static int reloadVeikkCtlDaemon(QCoreApplication &app)
{
	QDBusInterface *intf = VeikkDBusUtil::getInterface();

	if (!VeikkDBusUtil::isDaemonRunning()) {
		qWarning() << "veikkctl daemon not running. starting it now";
		return startVeikkCtlDaemon(app);
	}

	intf->call("reload");
	delete intf;
	return 0;
}

// runs the configuration gui
static int startConfigurationGui(QApplication &app)
{
	MainWindow win;

	win.show();
	return app.exec();
}

int main(int argc, char *argv[])
{
	QApplication app{argc, argv};
	QCommandLineParser parser;
	QString command;

	app.setApplicationName("veikkctl");
	app.setApplicationVersion("v3-alpha");

	parser.setApplicationDescription("VEIKK Digitizer Configuration Tool");
	parser.addPositionalArgument("ACTION",
			"The action to perform. This can be one of:\n"
			"  start    Start daemon\n"
			"  quit     Stop daemon\n"
			"  reload   Reload daemon\n"
			"  gui      Launch configuration GUI\n"
			"  help     Show this help menu\n"
			"  version  Show version info\n");
	parser.process(app);

	if (parser.positionalArguments().length() == 0) {
		parser.showHelp();
		return 0;
	}
	command = parser.positionalArguments().first();

	if (command == "start")
		return startVeikkCtlDaemon(app);
	if (command == "quit")
		return quitVeikkCtlDaemon();
	if (command == "reload")
		return reloadVeikkCtlDaemon(app);
	if (command == "gui")
		return startConfigurationGui(app);
	if (command == "version") {
		parser.showVersion();
		return 0;
	}

	// if no options selected, show help menu
	parser.showHelp();
	return 0;
}
