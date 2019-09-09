#include "MailSender.h"

#include <iostream>
#include <string>

using namespace std;

#define CURL_STATICLIB

#include <curl/curl.h>

MailSender::MailSender(string server, string user)
{
	this->server = server;
	this->user = user;

	this->curl = curl_easy_init();
}

MailSender::~MailSender()
{
	if (this->curl) {
		curl_easy_cleanup(this->curl);
	}
}

int MailSender::send(string from, string to, string subject, string textPlain, string textHtml, string file)
{
	CURLcode res = CURLE_OK;
	if (this->curl) {
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
		string subjectHeader = "Subject: ";
		subjectHeader += subject;

		//Zuvor definierte header in Headerliste einfuegen
		headers = curl_slist_append(headers, fromHeader.c_str());


		/*
			Liste der Empfänger fuellen. Muessen mit Liste der Empfaenger im header uebereinstimmen.
		*/
		recipients = curl_slist_append(recipients, to.c_str());
		headers = curl_slist_append(headers, toHeader.c_str());


		headers = curl_slist_append(headers, subjectHeader.c_str());

		/*
			Addresse oder URL des Mailservers mit Protokoll und ggf Port

			smtp://serveradresse:port
		*/
		curl_easy_setopt(this->curl, CURLOPT_URL, this->server.c_str());

		// Versuche SSL, falls nicht möglich mache weiter ohne
		curl_easy_setopt(this->curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

		/*
			Authentifizierung beim Server.
			Format: user:password
		*/
		curl_easy_setopt(this->curl, CURLOPT_USERPWD, this->user.c_str());

		// Absender angeben, muss mit Empfaenger in header uebereinstimmen
		curl_easy_setopt(this->curl, CURLOPT_MAIL_FROM, from.c_str());
		curl_easy_setopt(this->curl, CURLOPT_MAIL_RCPT, recipients);

		// Headerliste zur anfrage hinzufuegen
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);

		/* Mime-Nachricht initialisieren */
		mime = curl_mime_init(this->curl);

		/* Text Nachricht einfuegen. */
		if (textPlain != "") {
			//Neuen Abschnitt in MIME anlegen.
			part = curl_mime_addpart(mime);
			//Text mit suffix versehen (wird ansonsten in einigen Mailclients falsch dargestellt
			string text = textPlain + "\r\n";
			//Text in MIME Abschnitt einfuegen
			curl_mime_data(part, text.c_str(), CURL_ZERO_TERMINATED);
			//Mime-Typ setzen
			curl_mime_type(part, "text/plain");
			//Codierung setzen
			curl_mime_encoder(part, "quoted-printable");
		}

		/* HTML Nachricht einfuegen. Siehe oben*/
		if (textHtml != "") {
			part = curl_mime_addpart(mime);
			string html = textHtml + "\r\n";
			curl_mime_data(part, html.c_str(), CURL_ZERO_TERMINATED);
			curl_mime_type(part, "text/html");
			curl_mime_encoder(part, "quoted-printable");
		}

		if (file != "") {
			/* Anhang erstellen */
			part = curl_mime_addpart(mime);
			// Datei einlesen und als Anhang setzen
			curl_mime_filedata(part, file.c_str());

			// Uebertragungscodierung setzen
			curl_mime_encoder(part, "base64");
		}

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
		curl_mime_free(mime);
	}
	else
	{
		res = CURLE_FAILED_INIT;
	}

	return res;
}
