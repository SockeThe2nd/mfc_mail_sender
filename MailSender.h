#include <string>

#include <curl/curl.h>

using namespace std;

#pragma once
class MailSender
{
private:
	string  server;
	string user;
	CURL* curl;

public:
	MailSender(string server, string user);
	~MailSender();

	int send(string from, string to, string subject , string textPlain = "", string textHtml = "", string file = "");
};

