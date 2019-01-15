#include "cms.h"

namespace cms {

//--------------------------------------------------------------------------------------------------
//----------          class ConnectionPool          ------------------------------------------------
//--------------------------------------------------------------------------------------------------

ConnectionPool::ConnectionPool() {
	oldCount = 0;
}

MySQL* ConnectionPool::init_connect() {
	MySQL *query = new MySQL();
	if (!query->init()) {
		printf("!query->init()\n");
		return NULL;
	}
	if (!query->connect("127.0.0.1", "root", "", "sitev")) {
		printf("!query->connect()\n");
		return NULL;
	}
	query->exec("SET NAMES utf8");
	return query;
}


MySQL* ConnectionPool::newConnection() {
	int count = pool.size();
	if (count > oldCount) {
		int a = 1;
	}
	oldCount = count;
	//printf("\nHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
	//printf("%d\n", count);
	//printf("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n\n");
	if (count == 0) {
		MySQL *query = init_connect();
		Connection *conn = new Connection();
		conn->enabled = true;
		conn->query = query;
		pool.insert(pool.end(), conn);
		int count = pool.size();
		return query;
	}
	else {
		for (int i = 0; i < count; i++) {
			if (pool[i]->enabled == false) {
				pool[i]->enabled = true;
				return pool[i]->query;
			}
		}
		MySQL *query = init_connect();
		Connection *conn = new Connection();
		conn->enabled = true;
		conn->query = query;
		pool.insert(pool.end(), conn);
		int count = pool.size();
		return query;
	}
	return NULL;
}

void ConnectionPool::deleteConnection(MySQL *query) {
	if (query == NULL) return;

	int count = pool.size();
	for (int i = 0; i < count; i++) {
		if (pool[i]->query == query) {
			pool[i]->enabled = false;
			return;
		}
	}
}

}
