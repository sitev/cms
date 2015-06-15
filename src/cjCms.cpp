#include "cj.h"
//#include "cjWeb.h"
#include "siteManager.h"
using namespace cj;

#ifdef OS_WINDOWS
#pragma comment(lib, "cjCore.lib")
#pragma comment(lib, "cjNetwork.lib")
#pragma comment(lib, "cjMySQL.lib")
#pragma comment (lib, "libmysql.lib")
#endif

int main(int argc, char* argv[])
{
	SiteManager *sm = new SiteManager(8080);
	printf("port = 8080\n");
	application = sm;


	MySQL *query = new MySQL();
	if (query->init()) {
		if (query->connect("127.0.0.1", "root", "123qwe", "test")) {
			if (query->exec("select * from mytab")) {
				if (query->storeResult()) {
					printf("row count = %d\n", query->getRowCount());
				}
			}
		}
	}

	sm->logger = new Logger("/var/log/cjCms.log");

	SiteManagerHandler *handler = new SiteManagerHandler(sm);
	sm->setHandler(handler);

	sm->run();


	return 0;
}
