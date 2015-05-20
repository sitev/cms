#include "cj.h"
//#include "cjWeb.h"
#include "siteManager.h"
using namespace cj;

#ifdef OS_WINDOWS
#pragma comment(lib, "cjCore.lib")
#endif

int main(int argc, char* argv[])
{

	SiteManager *sm = new SiteManager(80);
	application = sm;

	sm->logger = new Logger("/var/log/cjCms.log");

	SiteManagerHandler *handler = new SiteManagerHandler(sm);
	sm->setHandler(handler);

	sm->run();

	return 0;
}
