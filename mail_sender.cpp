// mail_sender.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

using namespace std;

#define CURL_STATICLIB

#include <curl/curl.h>

#ifdef _DEBUG
#	pragma comment (lib, "curl/libcurl_a_debug.lib")
#else
#	pragma comment (lib, "curl/libcurl_a.lib")
#endif


static const string MAIL_SERVER = "smtp://smtp.gmx.net:587";

int sendMail(string user, string from, string  to, string file)
{
	CURL* curl;
	CURLcode res = CURLE_OK;

	// libcurl initialisieren
	curl = curl_easy_init();
	if (curl) {
		// benoetigte variablen anlegen
		struct curl_slist* headers = NULL;
		struct curl_slist* recipients = NULL;
		struct curl_slist* slist = NULL;
		curl_mime* mime;
		curl_mimepart* part;

		/*
			SMTP-Header aus Parametern zusammensetzen zusammensetzen

			Siehe https://tools.ietf.org/html/rfc2076
		*/
		string toHeader = "To: ";
		toHeader += to;
		string fromHeader = "From: ";
		fromHeader += from;
		string subjectHeader = "Subject: Process completed.";

		//Zuvor definierte header in Headerliste einfuegen
		headers = curl_slist_append(headers, fromHeader.c_str());
		headers = curl_slist_append(headers, toHeader.c_str());
		headers = curl_slist_append(headers, subjectHeader.c_str());

		// Nachrichtentext als Klartext
		static const string msg_text[] = {
			"Dies ist der Nachrichtentext der Mail.\r\n"
			"\r\n"
			"Dieser wird in der Regel nur von Mailclients dargestellt die kein HTML rendern koennen.\r\n"
		};

		static const string msg_html[] = {
			"<html>\r\n"
			"<body>\r\n"
			"<p>Dies ist der <b>HTML</b>-Teil der Nachricht.</p>"
			"<br />\r\n"
			"<p>Wenn alles funktioniert ist <b>DIESER TEXT</b> fett gedruckt.</p>"
			"</body>\r\n"
			"</html>\r\n"
		};

		/* 
			Addresse oder URL des Mailservers mit Protokoll und ggf Port

			smtp://serveradresse:port
		*/
		curl_easy_setopt(curl, CURLOPT_URL, MAIL_SERVER.c_str());

		// Versuche SSL, falls nicht möglich mache weiter ohne
		curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

		/*
			Authentifizierung beim Server. 
			Format: user:password
		*/
		curl_easy_setopt(curl, CURLOPT_USERPWD, user.c_str());

		// Absender angeben, muss mit Empfaenger in header uebereinstimmen
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());

		/*
			Liste der Empfänger fuellen. Muessen mit Liste der Empfaenger im header uebereinstimmen.
		*/
		recipients = curl_slist_append(recipients, to.c_str());
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		// Headerliste yur anfrage hinzufuegen
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		/* Mime-Nachricht initialisieren */
		mime = curl_mime_init(curl);

		/* HTML Nachricht einfuegen. */
		part = curl_mime_addpart(mime);
		curl_mime_data(part, msg_html->c_str(), CURL_ZERO_TERMINATED);
		curl_mime_type(part, "text/html");

		/* Text Nachricht einfuegen. */
		part = curl_mime_addpart(mime);
		curl_mime_data(part, msg_text->c_str(), CURL_ZERO_TERMINATED);

		/* Anhang erstellen */
		part = curl_mime_addpart(mime);

		// Datei einlesen und als Anhang setzen
		curl_mime_filedata(part, file.c_str());

		// Uebertragungscodierung setzen
		curl_mime_encoder(part, "base64");

		// Mime-Nachricht zur Anfrage hinzufuegen
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

		/* Anfrage senden */
		res = curl_easy_perform(curl);

		/* Auf fehler bei uebertragung pruefen; Hier ggf error werfen */
		if (res != CURLE_OK)
			std::cout << "CURL Fehlgeschlagen: " << curl_easy_strerror(res) << "\n";

		/* Speicher wieder freigeben */
		curl_slist_free_all(recipients);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
		curl_mime_free(mime);
	}

	return (int)res;
}

int main(void)
{
	return sendMail("user:password", "roland.paltz@gmx.net", "katharina@clan-erbe.de", "Image.png");
};