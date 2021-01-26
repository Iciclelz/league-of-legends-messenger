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
#include "xmppclient.h"
#include <sstream>
#include "statusxml.h"
#include <QSettings>
#include <QInputDialog>
#include <QDir>
#include <QLineEdit>

XmppClient::XmppClient()
{
    QSettings settings("Ryukuo", "Poro");
    QString key(settings.value("key").toByteArray());
    if (key.isNull() || key.isEmpty()) {
        key = QInputDialog::getText(0, "Poro", "Enter your API Key: ");
    }
    SetApiKey(key.toUtf8().constData());

    isConnected = false;
    regionId = 0;
}

void XmppClient::SetApiKey(std::string key)
{
    this->key = key;

    QSettings settings("Ryukuo", "Poro");
    settings.setValue("key", QString::fromStdString(key));
}

void XmppClient::setRegion(int regionId)
{
    this->regionId = regionId;
}

void XmppClient::Connect(std::string username, std::string password)
{
    this->username = username;
    this->password = password;

    client = new Client(JID (std::string(username + "@pvp.net/xiff")), std::string("AIR_" + password));
    connectionTcpClient = new ConnectionTCPClient( client->logInstance(), (QString("chat." + region() + ".lol.riotgames.com")).toUtf8().constData(), 5223);
    connectionTls = new ConnectionTLS( client, connectionTcpClient, client->logInstance());
    client->setConnectionImpl(connectionTls);
    client->setCompression(true);
    client->registerMessageHandler(this);
    client->registerPresenceHandler(this);
    client->connect(false);
    xmppHandler = new std::thread(ClientReceiveUpdate, client);

    client->setPresence (Presence::Chat, 1, MakeStatus(1, "", 0, 0, 0, "", 0, 0, 0, "", "", "", 0));
    client->rosterManager()->registerRosterListener(this);
    isConnected = true;
}

void XmppClient::Disconnect()
{
    client->disconnect();
    delete client;
    delete connectionTcpClient;
    delete connectionTls;
}

std::string XmppClient::MakeStatus(int profileIcon, std::string statusMsg, int level, int wins, int leaves, std::string queueType, int rankedWins, int rankedLosses, int rankedRating, std::string tier, std::string skinname, std::string gameStatus, unsigned int timeStamp)
{
    std::ostringstream os ;
    os << "<body><profileIcon>" <<
          profileIcon << "</profileIcon><statusMsg>" <<
          statusMsg << "</statusMsg><level>" <<
          level << "</level><wins>" <<
          wins << "</wins><leaves>" <<
          leaves << "</leaves><queueType>" <<
          queueType << "</queueType><rankedWins>" <<
          rankedWins << "</rankedWins><rankedLosses>" <<
          rankedLosses << "</rankedLosses><rankedRating>" <<
          rankedRating << "</rankedRating><tier>" <<
          tier << "</tier><skinname>" <<
          skinname << "</skinname><gameStatus>" <<
          gameStatus << "</gameStatus><timeStamp>" <<
          timeStamp << "</timeStamp></body>";

    std::string status(os.str());

    os.str("");

    return status;
}

void XmppClient::ClientReceiveUpdate(Client *client)
{
    ConnectionError connectionError = ConnNoError;
    while (connectionError == ConnNoError) {
        connectionError = client->recv(1);
    }
}

QString XmppClient::region()
{
    switch (regionId) {
    case 0:
        return "na2"; //192.64.174.69
    case 1:
        return "eu";
    case 2:
        return "eun1";
    case 3:
        return "oc1";
    case 4:
        return "kr";
    case 5:
        return "br";
    case 6:
        return "ru";
    case 7:
        return "tr";
    }

    return "na2";
}

QString XmppClient::apiRegion()
{
    switch (regionId) {
    case 0:
        return "na";
    case 1:
        return "euw";
    case 2:
        return "eune";
    case 3:
        return "oce";
    case 4:
        return "kr";
    case 5:
        return "br";
    case 6:
        return "ru";
    case 7:
        return "tr";
    }

    return "na";
}

QString XmppClient::ReformatStatus(QString status)
{
    StatusXml xml(status);
    return xml.formatted_status();
}
