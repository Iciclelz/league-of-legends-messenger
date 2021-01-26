/*
    Copyright (C) 2016 Ryukuo

    This file is part of Poro.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef XMPPCLIENT_H
#define XMPPCLIENT_H
#include <string>
#include <thread>

#include <gloox/client.h>
#include <gloox/connectiontcpclient.h>
#include <gloox/connectiontls.h>
#include <gloox/messagehandler.h>
#include <gloox/presencehandler.h>
#include <gloox/message.h>
#include <gloox/jid.h>
#include <gloox/rosterlistener.h>
#include <gloox/connectionlistener.h>
#include <gloox/rostermanager.h>

#include <QString>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QObject>
#include <QMessageBox>

#include <windows.h>
#include <wininet.h>

using namespace gloox;

class XmppClient : public MessageHandler, PresenceHandler, RosterListener, ConnectionListener {
private:


    ConnectionTLS *connectionTls;
    ConnectionTCPClient *connectionTcpClient;

    std::string key;
    std::string username;
    std::string password;

    int regionId;
public:
    XmppClient();
    Client *client;
    std::thread *xmppHandler;

    bool isConnected;
    void SetApiKey(std::string key);
    void setRegion(int regionId);
    void Connect(std::string username, std::string password);
    void Disconnect();

    std::string MakeStatus(int profileIcon, std::string statusMsg, int level, int wins, int leaves, std::string queueType, int rankedWins, int rankedLosses, int rankedRating, std::string tier, std::string skinname, std::string gameStatus, unsigned int timeStamp);

public:
    void static ClientReceiveUpdate(Client *client);
    QString region();
    QString apiRegion();

    QString ReformatStatus(QString status);



public:
    virtual void handleMessage(const Message& stanza, MessageSession* session = 0) = 0;
    virtual void handlePresence(const Presence& presence) = 0;
    virtual void handleItemAdded (const JID &jid) {

    }

    virtual void handleItemSubscribed (const JID &jid){}

    virtual void handleItemRemoved (const JID &jid){}

    virtual void handleItemUpdated (const JID &jid){}

    virtual void handleItemUnsubscribed (const JID &jid){}

    virtual void handleRoster (const Roster &roster) = 0;

    virtual void handleRosterPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg){}

    virtual void handleSelfPresence (const RosterItem &item, const std::string &resource, Presence::PresenceType presence, const std::string &msg){}

	virtual bool handleSubscriptionRequest(const JID &jid, const std::string &msg){ return true; }

	virtual bool handleUnsubscriptionRequest(const JID &jid, const std::string &msg){ return true; }

    virtual void handleNonrosterPresence (const Presence &presence){}

    virtual void handleRosterError (const IQ &iq){}

    virtual void onConnect() { isConnected = true; }
    virtual bool onTLSConnect( const CertInfo& info ) { return true; }
    virtual void onDisconnect(ConnectionError e) { isConnected = false; }

public:

    QString getSummonerName(QString summonerId) {

        QString html(GetHtmlText("https://" + apiRegion() + ".api.pvp.net/api/lol/" + apiRegion() + "/v1.4/summoner/" + summonerId.mid(3, summonerId.size()) + "/name?api_key=" + QString::fromStdString(key)));
        int begin = html.indexOf("\":\"") + 3;
        return html.mid(begin, html.indexOf("\"}", begin) - begin);

    }

    QString getSummonerId(QString summonerName) {
        QString html(GetHtmlText("https://" + apiRegion() + ".api.pvp.net/api/lol/" + apiRegion() + "/v1.4/summoner/by-name/" + summonerName + "?api_key=" + QString::fromStdString(key)));
        int begin = html.indexOf("\"id\":") + 5;
        return html.mid(begin, html.indexOf(",", begin) - begin);
    }

    QString GetHtmlText(QString url) {
        HINTERNET h_internet = InternetOpenUrlA(InternetOpenA("PoroAgent", LOCAL_INTERNET_ACCESS, NULL, 0, 0), url.toUtf8().constData(), NULL, NULL, NULL, NULL);
        char szDownloadedString[512] = { 0 };
        DWORD dwNumberOfBytesRead = 0;

        InternetReadFile(h_internet, szDownloadedString, sizeof(szDownloadedString), &dwNumberOfBytesRead);

        if (h_internet) {
              InternetCloseHandle(h_internet);
        }

        return QString::fromStdString(std::string (szDownloadedString));

        /*QNetworkAccessManager manager;
        QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url)));
        QEventLoop event;
        QObject::connect(response, SIGNAL(finished()), &event, SLOT(quit()));
        event.exec();
        return QString(response->readAll());*/
    }


};


#endif // XMPPCLIENT_H
