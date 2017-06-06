#pragma once

#include "core.h"
#include "cms.h"

namespace cms {

class SiteManager;

enum UserType { utNone, utAdmin, tuUser };

class UserModule : public WebModule {
public:
	UserModule(SiteManager *manager);
	String generateContent(WebPage *page, HttpRequest &request);

	virtual void paint(WebPage *page, HttpRequest &request);
	virtual void paintAbout(WebPage *page, HttpRequest &request);
	virtual void paintRecovery(WebPage *page, HttpRequest &request);
	virtual void paintSignup(WebPage *page, HttpRequest &request);

	virtual void reset(WebPage *page, HttpRequest &request);
	virtual void sendAccount(WebPage *page, HttpRequest &request);

	virtual void activate(WebPage *page, HttpRequest &request);
	virtual void changePassword(WebPage *page, HttpRequest &request);

	virtual void ajax(WebPage *page, HttpRequest &request);
	virtual void ajaxIsEmailExist(WebPage *page, HttpRequest &request);
};

}
