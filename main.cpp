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

	app.setApplicationName("veikkctl");
	app.setApplicationVersion("v3-alpha");

	// very simple command-line parsing; only chooses the first option
	// in the list below if multiple options are set
	parser.setApplicationDescription("VEIKK Digitizer Configuration Tool");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addOptions({
		{{"s", "start"}, "Start mapping daemon"},
		{{"q", "quit"}, "Quit mapping daemon"},
		{{"r", "reload"}, "New device or configuration change"},
		{{"g", "gui"}, "Start configuration GUI"}
	});
	parser.process(app);

	if (parser.optionNames().size() > 1) {
		qWarning() << "only one option can be set at a time";
		return -1;
	}

	if (parser.isSet("start"))
		return startVeikkCtlDaemon(app);
	if (parser.isSet("quit"))
		return quitVeikkCtlDaemon();
	if (parser.isSet("reload"))
		return reloadVeikkCtlDaemon(app);
	if (parser.isSet("gui"))
		return startConfigurationGui(app);

	// if no options selected, show help menu
	parser.showHelp();
	return 0;
}
