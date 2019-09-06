// mail_sender.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

#include "MailSender.h"

using namespace std;

static const string MAIL_SERVER = "smtp://smtp.gmx.net:587";
static const string USER = "user:pass";

int main(void)
{
	MailSender sender = MailSender(MAIL_SERVER, USER);

	
	sender.send("roland.paltz@gmx.net", "roland.paltz@gmx.net", "THIS IS CUSTOM PLAINTEXT", "<b>THIS IS CUSTOM HTML<\b>", "Image.png");
	sender.send("roland.paltz@gmx.net", "roland.paltz@gmx.net", "", "", "Image.png");
	return sender.send("roland.paltz@gmx.net", "roland.paltz@gmx.net");
};