#include <iostream>
#include <string>

#include "MailSender.h"

using namespace std;

static const string MAIL_SERVER = "smtp://smtp.gmx.net:587";
static const string USER = "user:password";

int main(void)
{
	MailSender sender = new MailSender(MAIL_SERVER, USER);

	return sender.send("roland.paltz@gmx.net", "katharina@clan-erbe.de", "Image.png");
};