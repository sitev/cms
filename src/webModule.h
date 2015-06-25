#pragma once

#include "cj.h"
#include "cjCms.h"

namespace cj {

class WebModule {
public:
	int moduleId;
	string name;
	string about;
	string text;
	WebModule(int moduleId, string name, string about, string text);
};

}