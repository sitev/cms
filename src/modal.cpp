#include "cjCms.h"

namespace cj {

Modal::Modal(String modalId, WebTemplate *tpl, String tag) {
	this->modalId = modalId;
	tplIndex = tpl;
	this->tag = tag;
}

void Modal::setCaption(String caption) {
	this->caption = caption;
}

void Modal::setContent(String content) {
	this->content = content;
}

void Modal::setButtons(String buttons) {
	this->buttons = buttons;
}


void Modal::paint() {
	SiteManager *manager = (SiteManager *)application;
	WebTemplate *tpl = new WebTemplate();
	String tplPath = manager->uiPath + "/modal/tpl.html";
	if (tpl->open(tplPath)) {
		tpl->out("modalId", modalId);
		tpl->out("caption", caption);
		tpl->out("content", content);
		tpl->out("buttons", buttons);
		tpl->exec();
		tplIndex->out(tag, tpl->html);
	}
	delete tpl;
}

}