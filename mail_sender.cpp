// mail_sender.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string.h>

#define CURL_STATICLIB

#include <curl/curl.h>

#ifdef _DEBUG
#	pragma comment (lib, "curl/libcurl_a_debug.lib")
#else
#	pragma comment (lib, "curl/libcurl_a.lib")
#endif

/* This is a simple example showing how to send mime mail using libcurl's SMTP
 * capabilities. For an example of using the multi interface please see
 * smtp-multi.c.
 *
 * Note that this example requires libcurl 7.56.0 or above.
 */

#define FROM    "<roland.paltz@gmx.net>"
#define TO      "<roland.paltz@gmx.net>"

static const char* headers_text[] = {
  "To: " TO,
  "From: " FROM " (Example User)",
  "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
	"rfcpedant.example.org>",
  "Subject: example sending a MIME-formatted message",
  NULL
};

static const char inline_text[] =
"This is the inline text message of the e-mail.\r\n"
"\r\n"
"  It could be a lot of lines that would be displayed in an e-mail\r\n"
"viewer that is not able to handle HTML.\r\n";

static const char inline_html[] =
"<html><body>\r\n"
"<p>This is the inline <b>HTML</b> message of the e-mail.</p>"
"<br />\r\n"
"<p>It could be a lot of HTML data that would be displayed by "
"e-mail viewers able to handle HTML.</p>"
"</body></html>\r\n";


int main(void)
{
	CURL* curl;
	CURLcode res = CURLE_OK;

	curl = curl_easy_init();
	if (curl) {
		struct curl_slist* headers = NULL;
		struct curl_slist* recipients = NULL;
		struct curl_slist* slist = NULL;
		curl_mime* mime;
		curl_mime* alt;
		curl_mimepart* part;
		const char** cpp;

		/* This is the URL for your mailserver */
		curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmx.net:587");
		curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
		curl_easy_setopt(curl, CURLOPT_USERPWD, "user:password");

		/* Note that this option isn't strictly required, omitting it will result
		 * in libcurl sending the MAIL FROM command with empty sender data. All
		 * autoresponses should have an empty reverse-path, and should be directed
		 * to the address in the reverse-path which triggered them. Otherwise,
		 * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
		 * details.
		 */
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);

		/* Add two recipients, in this particular case they correspond to the
		 * To: and Cc: addressees in the header, but they could be any kind of
		 * recipient. */
		recipients = curl_slist_append(recipients, TO);
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		/* Build and set the message header list. */
		for (cpp = headers_text; *cpp; cpp++)
			headers = curl_slist_append(headers, *cpp);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		/* Build the mime message. */
		mime = curl_mime_init(curl);

		/* The inline part is an alternative proposing the html and the text
		   versions of the e-mail. */
		alt = curl_mime_init(curl);

		/* HTML message. */
		part = curl_mime_addpart(alt);
		curl_mime_data(part, inline_html, CURL_ZERO_TERMINATED);
		curl_mime_type(part, "text/html");

		/* Text message. */
		part = curl_mime_addpart(alt);
		curl_mime_data(part, inline_text, CURL_ZERO_TERMINATED);

		/* Create the inline part. */
		part = curl_mime_addpart(mime);
		curl_mime_subparts(part, alt);
		curl_mime_type(part, "multipart/alternative");
		slist = curl_slist_append(NULL, "Content-Disposition: inline");
		curl_mime_headers(part, slist, 1);

		/* Add the current source program as an attachment. */
		part = curl_mime_addpart(mime);
		curl_mime_filedata(part, "Image.png");
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

		/* Send the message */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK)
			std::cout << "curl_easy_perform() failed: "<< curl_easy_strerror(res) << "\n";

		/* Free lists. */
		curl_slist_free_all(recipients);
		curl_slist_free_all(headers);

		/* curl won't send the QUIT command until you call cleanup, so you should
		 * be able to re-use this connection for additional messages (setting
		 * CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and calling
		 * curl_easy_perform() again. It may not be a good idea to keep the
		 * connection open for a very long time though (more than a few minutes
		 * may result in the server timing out the connection), and you do want to
		 * clean up in the end.
		 */
		curl_easy_cleanup(curl);

		/* Free multipart message. */
		curl_mime_free(mime);
	}

	return (int)res;
}