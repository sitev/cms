#include "webModule.h"
namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebModule          -----------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebModule::WebModule(int moduleId, string name, string about, string text) {
	this->moduleId = moduleId;
	this->name = name;
	this->about = about;
	this->text = text;
}

}
