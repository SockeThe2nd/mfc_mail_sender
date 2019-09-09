# mfc_mail_sender

Versendet Mails mit Anhang aus einer C++-Anwendung mit Hilfe von libcurl (siehe [hier](https://curl.haxx.se/libcurl/))

## libcurl

Libcurl ist die open-source lib von curl, dem Standardtool für http/ftp/smtp.

Die Lizenz von curl erlaubt dabei die freie (auch komerzielle) Nutzung.

In diesem Fall habei ich eine statische Lib von libcurl aus dem [Quellcode](https://curl.haxx.se/download.html) erstellt
und in VS hinzugefügt.
Anleitung [HIER](https://curl.haxx.se/docs/install.html)

Einfacher sollte jedoch das Hinzufügen von libcurl mittels dem von microsoft bereitgestellten [vspkg](https://github.com/Microsoft/vcpkg).
Anleitung [HIER](https://stackoverflow.com/questions/47878888/getting-curl-to-work-with-visual-studios-2017)

Wichtig ist dabei das nicht das Basis-Curl-Packet sondern ein Packet mit SMTP-Support installiert wird.
