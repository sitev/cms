#pragma once

#include "cj.h"
#include "cjCms.h"

#include "vector"

namespace cj {

class Connection {
public:
	bool enabled;
	MySQL *query;
};

class ConnectionPool {
	int oldCount;
	virtual MySQL* init_connect();
public:
	std::vector<Connection*> pool;
	ConnectionPool();
	virtual MySQL* newConnection();
	virtual void deleteConnection(MySQL *query);
};

}