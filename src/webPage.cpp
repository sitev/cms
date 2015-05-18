#include "webPage.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebPage          -------------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebPage::WebPage() {
	moduleId = 0; //0 - никакой модуль не привязан
}

void WebPage::paintBegin() {
	this->out("HTTP/1.1 200 OK\r\n");
	this->out("Content-type: text/html; charset=UTF-8\r\n");
	String s = createParamString(ptCOOKIE);
	if (s != "") {
		if (cookieSavingTime == 0)
			this->out((String)"Set-Cookie: " + s + /*"; expires=" + st +*/ "; path=/ \n");
		else {
			time_t t = time(NULL);
			String st = getCookieFormatTime(t + cookieSavingTime);
			this->out((String)"Set-Cookie: " + s + "; expires=" + st + "; path=/ \n");
		}
	}

	//this->out("\n");
}
void WebPage::paint() {
	if (url == "") return;
	string url8 = url.toString8();
	File *f = new File(url, "r");
	String s, t;
	f->readAll(s);
	tplIndex->changeAllTag(s, t);
	this->out("Content-Length: " + (String)(t.getLength()) + "\r\n\r\n" + t + "\r\n");
	//this->out("Content-Length: " + (String)(t.toString8().length()) + "\r\n\r\n" + t + "\r\n");
	this->out(t);
	application->logger_out(t);
	delete f;
}
void WebPage::paintEnd() {
}
void WebPage::repaint() {
	paintBegin();
	paint();
	paintEnd();
}
void WebPage::out(String s) {
}
void WebPage::out(String tag, String s) {
	tplIndex->out(tag, s);
}

}
