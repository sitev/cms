#pragma once

namespace cj {

class WebPage {
public:
	int moduleId;
	WebPage();
	virtual void paintBegin();
	virtual void paint();
	virtual void paintEnd();
	virtual void repaint();
};

}
