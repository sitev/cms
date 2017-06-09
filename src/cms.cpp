#include "cms.h"
using namespace cms;

#ifdef OS_WINDOWS
#pragma comment(lib, "core.lib")
#pragma comment(lib, "network.lib")
#pragma comment(lib, "mysql.lib")
#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "webserver.lib")

#pragma comment(lib, "Ole32.lib")
#endif

int main(int argc, char* argv[])
{
	SiteManager *sm = new SiteManager(8080);
	printf("port = 8080\n");
	application = sm;

	sm->logger = new Logger("/var/log/cms.log");
	sm->logger->out("Start cms!");

	SiteManagerHandler *handler = new SiteManagerHandler(sm);
	sm->setHandler(handler);

	sm->run();

	return 0;
}
