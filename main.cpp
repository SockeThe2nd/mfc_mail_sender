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
	/*
	 * Instanz der MailSender-Class erstellen.
	 * Die Adresse des Mailservers mit Port sowie der zu verwendende Benutzer sind als Argumente angegeben.
	 */
	MailSender sender = MailSender(MAIL_SERVER, USER);

	//Senden einer Mail. Der MailSender kann mehrfach verwendet werden sollen mehrere Mails versandt werden. 
	sender.send("roland.paltz@gmx.net", "roland.paltz@gmx.net", "Test-Betreff", "Inhalt als text", "<b>Inhalt als HTML<\b>", "Image.png");
	//Sollen nur text-Mails versendet werden, so kann der HTML-Parameter leer gelassen werden.
	sender.send("roland.paltz@gmx.net", "roland.paltz@gmx.net", "Test-Betreff ohne HTML", "Inhalt als Text", "", "Image.png");
	return sender.send("roland.paltz@gmx.net", "roland.paltz@gmx.net", "Test-Betreff ohne Inhalt");
};