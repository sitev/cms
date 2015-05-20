#define _CRT_SECURE_NO_WARNINGS

#include "webServer.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class RequestHeader          -------------------------------------------------
//--------------------------------------------------------------------------------------------------

RequestHeader::RequestHeader() {

}
bool RequestHeader::parse(String request) {
	/*
	isFileFlag = false;
	clear();

	LOGGER_TRACE("request = " + request);
	int pos1 = request.getPos(" ");
	if (pos1 <= 0) return false;
	String method = request.subString(0, pos1);

	add("Method", method);

	LOGGER_TRACE("method = " + method);

	String httpstr = " HTTP/1.1\r\n";
	int pos2 = request.getPos(httpstr);
	if (pos2 <= pos1) return false;
	string sParams = request.subString(pos1 + 2, pos2 - pos1 - 2);
	string sss = decodeCp1251(sParams);

	add("Params", sss);

	if (isFile(sParams, fileExt)) {
		isFileFlag = true;
	}
	else parseParams(sParams);

	add("Version", (String)"1.1");

	String s = request.subString(pos2 + httpstr.getLength());
	while (true) {
		string s8 = s.toString8();
		pos1 = s.getPos(":");
		if (pos1 <= 0) break;
		String name = s.subString(0, pos1);
		string n8 = name.toString8();

		s = s.subString(pos1 + 2);

		pos1 = s.getPos("\r\n");
		if (pos1 <= 0) break;

		String value = s.subString(0, pos1);
		string v8 = value.toString8();

		add(name, value);

		s = s.subString(pos1 + 2);
	}
	*/
	return true;
}

bool RequestHeader::parse(Memory request) {
	isFileFlag = false;
	clear();

	LOGGER_TRACE("request = " + request);
	int pos1 = find(request, ' ');
	if (pos1 <= 0) return false;

	string method = substr(request, 0, pos1);
	add("Method", method);

	LOGGER_TRACE("method = " + method);

	pos1++;
	string httpstr = " HTTP/1.1\r\n";
	int pos2 = find(request, httpstr);
	if (pos2 <= pos1) return false;
	char ch = getChar(request, pos1);
	if (ch != '/') return false;
	ch = getChar(request, pos1 + 1);
	if (ch == '?') pos1++;

	string sParams = substr(request, pos1 + 1, pos2 - pos1 - 1);
	string sDecode = decodeCp1251(sParams);
	add("Params", sDecode);

	if (isFile(sParams, fileExt)) {
		isFileFlag = true;
	}
	else 
		parseParams(sParams);

	add("Version", (string)"1.1");

	while (true) {
		pos1 = request.getPos();
		pos2 = find(request, ":");
		if (pos2 < 0) break;
		int pos3 = find(request, "\r\n");
		if (pos3 <= 0) break;
		string name = substr(request, pos1, pos2 - pos1);
		string value = substr(request, pos2 + 2, pos3 - pos2 - 2);
		add(name, value);
		//break;
	}

	/*
	String s = substr(request, pos2 + httpstr.length());
	while (true) {
		string s8 = s.toString8();
		pos1 = s.getPos(":");
		if (pos1 <= 0) break;
		String name = s.subString(0, pos1);
		string n8 = name.toString8();

		s = s.subString(pos1 + 2);

		pos1 = s.getPos("\r\n");
		if (pos1 <= 0) break;

		String value = s.subString(0, pos1);
		string v8 = value.toString8();

		add(name, value);

		s = s.subString(pos1 + 2);
	}
	*/

	return true;
}

bool RequestHeader::parseParams(String sss) {
	params.clear();
	if (sss == "") return true;

	string s = sss.toString8();

	int pos = 0;
	string tmp = s.substr(pos, 1);
	if (tmp == "/") {
		pos++;
	}

	tmp = s.substr(pos, 1);
	if (tmp == "?") {
		pos++;
	}

	string par = s.substr(pos);

	pos = 0;
	while (1) {
		int pos1 = par.find("=", pos);
		string name = par.substr(pos, pos1 - pos);
		//printf("name = %s & ", name.c_str());
		pos = pos1 + 1;

		int pos2 = par.find("&", pos);
		if (pos2 == -1) pos2 = par.size();
		string value = par.substr(pos, pos2 - pos);
		//printf("value = %s\n", value.c_str());
		pos = pos2 + 1;

		params.insert(name, value);

		//GETHandler(name, value);

		if (pos2 == par.size()) break;
	}

	//for(map<string, string>::iterator i = pars.begin(); i != pars.end(); i++) {
		//printf("/// name = %s & value = %s /// \n", (*i).first.c_str(), (*i).second.c_str());
	//}

	return true;
}

bool RequestHeader::isFile(string s, string &fileExt) {
	int pos = s.find_last_of('.');
	if (pos < 0) return false;
	fileExt = s.substr(pos + 1);
	return true;
}

string urlDecode(string &SRC) {
	string ret;
	char ch;
	int i, ii;
	for (i = 0; i<SRC.length(); i++) {
		if (int(SRC[i]) == 37) {
			sscanf(SRC.substr(i + 1, 2).c_str(), "%x", &ii);
			ch = static_cast<char>(ii);
			ret += ch;
			i = i + 2;
		}
		else {
			ret += SRC[i];
		}
	}
	return (ret);
}

#define LINE_MAX 1024

typedef struct ConvLetter {
	char    win1251;
	int             unicode;
} Letter;


static Letter g_letters[] = {
	{ 0x82, 0x201A }, // SINGLE LOW-9 QUOTATION MARK
	{ 0x83, 0x0453 }, // CYRILLIC SMALL LETTER GJE
	{ 0x84, 0x201E }, // DOUBLE LOW-9 QUOTATION MARK
	{ 0x85, 0x2026 }, // HORIZONTAL ELLIPSIS
	{ 0x86, 0x2020 }, // DAGGER
	{ 0x87, 0x2021 }, // DOUBLE DAGGER
	{ 0x88, 0x20AC }, // EURO SIGN
	{ 0x89, 0x2030 }, // PER MILLE SIGN
	{ 0x8A, 0x0409 }, // CYRILLIC CAPITAL LETTER LJE
	{ 0x8B, 0x2039 }, // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
	{ 0x8C, 0x040A }, // CYRILLIC CAPITAL LETTER NJE
	{ 0x8D, 0x040C }, // CYRILLIC CAPITAL LETTER KJE
	{ 0x8E, 0x040B }, // CYRILLIC CAPITAL LETTER TSHE
	{ 0x8F, 0x040F }, // CYRILLIC CAPITAL LETTER DZHE
	{ 0x90, 0x0452 }, // CYRILLIC SMALL LETTER DJE
	{ 0x91, 0x2018 }, // LEFT SINGLE QUOTATION MARK
	{ 0x92, 0x2019 }, // RIGHT SINGLE QUOTATION MARK
	{ 0x93, 0x201C }, // LEFT DOUBLE QUOTATION MARK
	{ 0x94, 0x201D }, // RIGHT DOUBLE QUOTATION MARK
	{ 0x95, 0x2022 }, // BULLET
	{ 0x96, 0x2013 }, // EN DASH
	{ 0x97, 0x2014 }, // EM DASH
	{ 0x99, 0x2122 }, // TRADE MARK SIGN
	{ 0x9A, 0x0459 }, // CYRILLIC SMALL LETTER LJE
	{ 0x9B, 0x203A }, // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
	{ 0x9C, 0x045A }, // CYRILLIC SMALL LETTER NJE
	{ 0x9D, 0x045C }, // CYRILLIC SMALL LETTER KJE
	{ 0x9E, 0x045B }, // CYRILLIC SMALL LETTER TSHE
	{ 0x9F, 0x045F }, // CYRILLIC SMALL LETTER DZHE
	{ 0xA0, 0x00A0 }, // NO-BREAK SPACE
	{ 0xA1, 0x040E }, // CYRILLIC CAPITAL LETTER SHORT U
	{ 0xA2, 0x045E }, // CYRILLIC SMALL LETTER SHORT U
	{ 0xA3, 0x0408 }, // CYRILLIC CAPITAL LETTER JE
	{ 0xA4, 0x00A4 }, // CURRENCY SIGN
	{ 0xA5, 0x0490 }, // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
	{ 0xA6, 0x00A6 }, // BROKEN BAR
	{ 0xA7, 0x00A7 }, // SECTION SIGN
	{ 0xA8, 0x0401 }, // CYRILLIC CAPITAL LETTER IO
	{ 0xA9, 0x00A9 }, // COPYRIGHT SIGN
	{ 0xAA, 0x0404 }, // CYRILLIC CAPITAL LETTER UKRAINIAN IE
	{ 0xAB, 0x00AB }, // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
	{ 0xAC, 0x00AC }, // NOT SIGN
	{ 0xAD, 0x00AD }, // SOFT HYPHEN
	{ 0xAE, 0x00AE }, // REGISTERED SIGN
	{ 0xAF, 0x0407 }, // CYRILLIC CAPITAL LETTER YI
	{ 0xB0, 0x00B0 }, // DEGREE SIGN
	{ 0xB1, 0x00B1 }, // PLUS-MINUS SIGN
	{ 0xB2, 0x0406 }, // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
	{ 0xB3, 0x0456 }, // CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
	{ 0xB4, 0x0491 }, // CYRILLIC SMALL LETTER GHE WITH UPTURN
	{ 0xB5, 0x00B5 }, // MICRO SIGN
	{ 0xB6, 0x00B6 }, // PILCROW SIGN
	{ 0xB7, 0x00B7 }, // MIDDLE DOT
	{ 0xB8, 0x0451 }, // CYRILLIC SMALL LETTER IO
	{ 0xB9, 0x2116 }, // NUMERO SIGN
	{ 0xBA, 0x0454 }, // CYRILLIC SMALL LETTER UKRAINIAN IE
	{ 0xBB, 0x00BB }, // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
	{ 0xBC, 0x0458 }, // CYRILLIC SMALL LETTER JE
	{ 0xBD, 0x0405 }, // CYRILLIC CAPITAL LETTER DZE
	{ 0xBE, 0x0455 }, // CYRILLIC SMALL LETTER DZE
	{ 0xBF, 0x0457 } // CYRILLIC SMALL LETTER YI
};

int convert_utf8_to_windows1251(const char* utf8, char* windows1251, size_t n)
{
	int i = 0;
	int j = 0;
	for (; i < (int)n && utf8[i] != 0; ++i) {
		char prefix = utf8[i];
		char suffix = utf8[i + 1];
		if ((prefix & 0x80) == 0) {
			windows1251[j] = (char)prefix;
			++j;
		}
		else if ((~prefix) & 0x20) {
			int first5bit = prefix & 0x1F;
			first5bit <<= 6;
			int sec6bit = suffix & 0x3F;
			int unicode_char = first5bit + sec6bit;


			if (unicode_char >= 0x410 && unicode_char <= 0x44F) {
				windows1251[j] = (char)(unicode_char - 0x350);
			}
			else if (unicode_char >= 0x80 && unicode_char <= 0xFF) {
				windows1251[j] = (char)(unicode_char);
			}
			else if (unicode_char >= 0x402 && unicode_char <= 0x403) {
				windows1251[j] = (char)(unicode_char - 0x382);
			}
			else {
				int count = sizeof(g_letters) / sizeof(Letter);
				for (int k = 0; k < count; ++k) {
					if (unicode_char == g_letters[k].unicode) {
						windows1251[j] = g_letters[k].win1251;
						goto NEXT_LETTER;
					}
				}
				// can't convert this char
				return 0;
			}
		NEXT_LETTER:
			++i;
			++j;
		}
		else {
			// can't convert this chars
			return 0;
		}
	}
	windows1251[j] = 0;
	return 1;
}

string RequestHeader::decodeCp1251(string s) {
	string utf8 = urlDecode(s);
	char win1251[LINE_MAX] = { 0 };
#ifdef OS_WINDOWS
	convert_utf8_to_windows1251(utf8.c_str(), win1251, LINE_MAX);
	return win1251;
#endif

#ifdef OS_LINUX
	return utf8;
#endif
	/*
	String res = "";
	int len = s.getLength();
	for (int i = 0; i < len; i++) {
		Char c = s.getChar(i);
		if (c.get() == '%') {
			String sCode = "0x" + s.subString(i + 1, 2);
			char code = sCode.toInt();
			res = res + (String)code;
		}
		else res = res + c.get();
	}
	return res;
	*/
}

int RequestHeader::find(Memory &request, char a) {
	while (!request.eof()) {
		char ch;
		request.read(&ch, 1);
		if (ch == a) return request.getPos() - 1;
	}

	return -1;
}

int RequestHeader::find(Memory &request, string s) {
	int index;
	int len = s.length();
	if (len <= 0) return -1;
	bool flag = false;
	while (!request.eof()) {
		if (!flag) {
			flag = find(request, s[0]);
			index = 1;
		}
		else {
			char ch;
			request.read(&ch, 1);
			char a = s[index];
			index++;
			if (ch != a) {
				flag = false;
				index = 0;
			}
		}
		if (len <= index) {
			if (flag) return request.getPos() - len;
		}
	}
	return -1;
}

string RequestHeader::substr(Memory &request, int pos, int count) {
	char *s = (char*)malloc(count + 1);
	memcpy(s, (char*)request.data + pos, count);
	s[count] = 0;
	string ret = s;
	free(s);
	return ret;
}

char RequestHeader::getChar(Memory &request, int pos) {
	return ((char*)request.data)[pos];
}



//--------------------------------------------------------------------------------------------------
//----------          class HttpRequest          ---------------------------------------------------
//--------------------------------------------------------------------------------------------------

void HttpRequest::parse() {
	LOGGER_TRACE("Start parse");
	String s = "";
	int size = memory.getSize();
//	s.setLength(size);
//	s = (char*)(memory.data);
	int t1 = 0;//GetTickCount();
	//printf("tick1 = %d\n", t1);
	/*
	for (int i = 0; i < size; i++) {
		char ch;
		memory.readChar(ch);
		s = s + ch;
	}
	*/
	int t2 = 0;//GetTickCount();
	//printf("tick2 = %d %d\n", t2, t2 - t1);
	//header.parse(s);
	header.parse(memory);

	int t3 = 0;//GetTickCount();
	//printf("tick3 = %d %d\n", t3, t3 - t2);
	LOGGER_TRACE("Finish parse");
}


//--------------------------------------------------------------------------------------------------
//----------          class WebServerHandler          ----------------------------------------------
//--------------------------------------------------------------------------------------------------
void WebServerHandler::threadStep(Socket *socket) {
	try {
		LOGGER_OUT("MUTEX", "application->g_mutex.lock(); {");
		application->g_mutex.lock();
		LOGGER_OUT("MUTEX", "application->g_mutex.lock(); }");
		request.memory.setPos(0);
		request.memory.setSize(0);
		response.memory.setPos(0);
		response.memory.setSize(0);

		//// recvAll {
		while (true) {
			int size = socket->getCurSize();
			if (size <= 0) break;
			socket->recv(request.memory);
		}
		//// }

		printf("----------\n");
		int count = request.memory.getSize();
		for (int i = 0; i < count; i++) {
			printf("%c", ((char*)request.memory.data)[i]);
		}
		printf("\n");
		request.parse();
		application->g_mutex.unlock();
		LOGGER_OUT("MUTEX", "application->g_mutex.unlock();");

		internalStep(request, response);

		LOGGER_OUT("MUTEX", "application->g_mutex.lock(); {");
		application->g_mutex.lock();
		LOGGER_OUT("MUTEX", "application->g_mutex.lock(); }");
		if (socket->sendAll(response.memory)) LOGGER_TRACE("sendAll OK!"); else LOGGER_TRACE("sendAll error ...");
		socket->close();
		delete socket;
		application->g_mutex.unlock();
	}
	catch(...) {
		LOGGER_ERROR("Error in threadStep try catch");
	}
}

void WebServerHandler::internalStep(HttpRequest &request, HttpResponse &response) {
	string host = request.header.getValue("Host").toString8();
	//host = "sitev.ru";

	if (!request.header.isFileFlag && isPageExist(host))
	{
		step(request, response);
	}
	else {
		string fn = request.header.getValue_s("Params");
		if (fn == "") fn = "index_tpl.html";
		fn = "/var/www/" + host + "/" + fn;
		printf("filename = %s\n", fn.c_str());
		File *f = new File(fn, "rb");
		bool flag = f->isOpen();
		if (flag) {
			string s = "HTTP/1.1 200 OK\r\nContent-Type: ";
			if (request.header.fileExt == "html") s = s + "text/html";
			else if (request.header.fileExt == "ico") s = s + "image/ico";
			else if (request.header.fileExt == "png") s = s + "image/png";
			else if (request.header.fileExt == "jpg") s = s + "image/jpeg";
			else if (request.header.fileExt == "js") s = s + "text/javascript";
			else if (request.header.fileExt == "css") s = s + "text/css";
			else if (request.header.fileExt == "gif") s = s + "image/gif";


			int sz = f->getSize();
			s = s + "\r\nContent - Length: " + to_string(sz) + "\r\n\r\n";
			response.memory.write((void*)(s.c_str()), s.length());

			Memory mem;
			mem.setSize(sz);
			f->read(mem.data, sz);
			response.memory.write(mem.data, sz);

			int a = 1;
		}
		delete f;
	}
}

void WebServerHandler::step(HttpRequest &request, HttpResponse &response) {
	int count = request.header.getCount();

	string s = "";
	for (int i = 0; i < count; i++) {
		string name = request.header.getName_s(i);
		string value = request.header.getValue_s(i);
		s = s + name + " = " + value + "\r\n";
	}

	count = request.header.params.getCount();
	s = s + to_string(count) + "\r\n";

	for (int i = 0; i < count; i++) {
		String name = request.header.params.getName(i);
		String value = request.header.params.getValue(i);
		s = s + name.toString8() + " = " + value.toString8() + "\r\n";
	}

	s = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(s.length()) + "\r\n\r\n" + s + "\r\n";
	response.memory.write((void*)(s.c_str()), s.length());
}



//--------------------------------------------------------------------------------------------------
//----------          class WebServer          -----------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebServer::WebServer(int port) {
	ss = new ServerSocket();
	socketPort = port;
}

mutex g_mutex1;

void WebServer::threadFunction(Socket *socket)
{
	g_mutex1.lock();
	WebServerHandler *handler = new WebServerHandler();
	cout << "new " << handler << endl;
	g_mutex1.unlock();

	handler->threadStep(socket);

	g_mutex1.lock();
	cout << "del " << handler << endl;
	delete handler;
	g_mutex1.unlock();
}


void WebServer::run() {
	try {
		isRunning = true;
		bool flag = ss->create(AF_INET, SOCK_STREAM, 0);
		if (!flag)
		{
			exit(1);
		}
		if (!ss->bind(socketPort))
		{
			exit(2);
		}

		ss->setNonBlocking(true);
		ss->listen();

		int cnt = 0;
		while (isRunning)
		{
			ss->accept();
			cnt++;

			int index = 0;
			int count = ss->lstSocket.getCount();

			if (cnt % 1000 == 0) {
				String s = "Accept. Count = " + (String)count;
				LOGGER_DEBUG(s);

				//int threadCount = lstThread.size();
				//LOGGER_OUT("THREAD", "Number of thread: " + (String)threadCount);
			}

			for (int i = 0; i < count; i++) {
				Socket *socket = (Socket*)ss->lstSocket.getItem(index);
				index++;

				int size = socket->getCurSize();
				//				LOGGER_OUT("SIZE", "Size of Socket buffer = " + (String)size);

				if (size > 0) {
					index--;
					ss->lstSocket.del(index);

					LOGGER_DEBUG("----- i = " + (String)i + " size = " + (String)size);

					//std::thread *thr = new std::thread(&WebServer::threadStep, this, socket);
					std::thread *thr = new std::thread(&WebServer::threadFunction, this, socket);
					thr->detach();
					//lstThread.push_back(thr);
				}
			}
			usleep(1000);
		}
	}
	catch (...) {
		printf("Error in run() try catch(...)\n");
	}
}


void WebServer::setHandler(WebServerHandler *handler) {
	this->handler = handler;
}

void WebServer::threadStep(Socket *socket) {
	if (handler) handler->threadStep(socket);
}

}
