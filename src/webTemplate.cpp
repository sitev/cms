#include "webTemplate.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          Класс ParamItem          -----------------------------------------------------
//--------------------------------------------------------------------------------------------------
ParamItem::ParamItem() {
	name = "";
	value = "";
}
void ParamItem::set(String name, String value) {
	this->name = name;
	this->value = value;
}
	
//--------------------------------------------------------------------------------------------------
//----------          class WebTemplate          ---------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebTemplate::WebTemplate() {
}

bool WebTemplate::open(String url) {
	File *f = new File(url, "r");
	if (!f->isOpen()) return false;
	f->readAll(html);
	delete f;

	return true;
}
void WebTemplate::out(String tag, String s) {
	ParamItem *pi = new ParamItem();
	pi->name = tag;
	pi->value = s;
	lstTag.add(pi);
}
String WebTemplate::getTagValue(String tag) {
	String s = "";
	int count = lstTag.getCount();
	for (int i = 0; i < count; i++) {
		ParamItem *pi = (ParamItem*)lstTag.getItem(i);
		if (pi->name == tag) s += pi->value;
	}
	return s;
}
void WebTemplate::exec(String source, String &target) {
	String s = source;
	target = "";
	int status = 0;
	int pos0 = 0, pos1, pos2;
	int len = s.getLength();
	for (int i = 0; i < len; i++) {
		Char ch = s.getChar(i);
		if (status == 0 && ch == '{') {
			pos1 = i;
			status = 1;
		}
		else if (status == 1) {
			if (ch == '}') {
				pos2 = i;
				String s1 = s.subString(pos0, pos1 - pos0);
				String tag = s.subString(pos1 + 1, pos2 - pos1 - 1);
				String value = getTagValue(tag);
				target += s1 + value;
				pos0 = pos2 + 1;
				status = 0;
			}
			else {
				if (ch >= 0 && ch <= 32) status = 0;
			}
		}
	}
	target += s.subString(pos0, s.getLength());
}

void WebTemplate::exec() {
	exec(html, html);
}


}
