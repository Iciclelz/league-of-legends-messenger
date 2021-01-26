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
#include "counterwidget.h"
#include "ui_counterwidget.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <string>

#define LEAGUE_OF_LEGENDS_VERSION "6.4.2"

CounterWidget::CounterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CounterWidget)
{
    ui->setupUi(this);

    pNetworkAccessManager = new QNetworkAccessManager(this);
    pNetworkAccessManager2 = new QNetworkAccessManager(this);
    pNetworkAccessManager3 = new QNetworkAccessManager(this);
    connect(pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(GetChampionNetworkReplyFinished(QNetworkReply*)));
    connect(pNetworkAccessManager2, SIGNAL(finished(QNetworkReply*)), this, SLOT(GetChampionImageFinished(QNetworkReply*)));
    connect(pNetworkAccessManager3, SIGNAL(finished(QNetworkReply*)), this, SLOT(GetChampionDetailsFinished(QNetworkReply*)));
    pNetworkAccessManager->get(QNetworkRequest(QUrl(
		"http://ddragon.leagueoflegends.com/cdn/"
		LEAGUE_OF_LEGENDS_VERSION
		"/data/en_US/champion.json")));
}

CounterWidget::~CounterWidget()
{
    //delete pChampionDetailsManager;
    delete pNetworkAccessManager;
    delete pNetworkAccessManager2;
    delete pNetworkAccessManager3;
    delete ui;
}

void CounterWidget::OnShow() {
    ui->label->setText("Ahri");

    for (uint i = 0; i < championinfo.size(); i++) {
        if (ui->label->text().compare(championinfo.at(i).qchampionname) == 0) {
            ui->label_2->setText(championinfo.at(i).qchampiondescription);
            ui->label_3->setPixmap(championinfo.at(i).qchampionpixmap);
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/weak")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/strong")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/good")));
            break;
        }
    }
}

void CounterWidget::GetChampionNetworkReplyFinished(QNetworkReply* pReply) {
    QJsonObject jsonObject = QJsonDocument::fromJson(pReply->readAll()).object()["data"].toObject();
    QStringList jsonStringList = jsonObject.keys();
    for (int i = 0; i < jsonObject.length(); i++) {
        QJsonObject obj = jsonObject[jsonStringList.at(i)].toObject();
        ChampionInfo info(obj["name"].toString(), obj["title"].toString(), obj["id"].toString());
        championinfo.push_back(info);

        ui->listWidget->addItem(info.qchampionname);
    }

    for(uint i = 0; i < championinfo.size(); i++) {
        pNetworkAccessManager2->get(QNetworkRequest(QUrl(
			"http://ddragon.leagueoflegends.com/cdn/"
			LEAGUE_OF_LEGENDS_VERSION
			"/img/champion/" + championinfo.at(i).qchampionid + ".png")));
    }


    pReply->deleteLater();
}

void CounterWidget::GetChampionImageFinished(QNetworkReply* pReply) {
    QPixmap pixmap;
    pixmap.loadFromData(pReply->readAll());
    for (uint i = 0; i < championinfo.size(); i++) {
        if (pReply->url().path().endsWith(championinfo.at(i).qchampionid + ".png")) {
            championinfo.at(i).setImage(pixmap);
            break;
        }
    }

    pReply->deleteLater();
}

void CounterWidget::GetChampionDetailsFinished(QNetworkReply* pReply) {

   int j = -1;
   if (pReply->url().path().endsWith("/weak")) {
        j = 0;
   }
   if (pReply->url().path().endsWith("/strong")) {
        j = 1;
   }
   if (pReply->url().path().endsWith("/good")) {
        j = 2;
   }
   QRegularExpression rx("<div class='name'>.*</div>");
   QRegularExpressionMatchIterator i = rx.globalMatch(QString(pReply->readAll()));
   i.next();
   while (i.hasNext()) {
       QRegularExpressionMatch match = i.next();
       if (match.hasMatch()) {
           std::string str = match.captured(0).trimmed().toUtf8().constData();
           str = str.substr(18, str.length() - 6 - 18);
           switch (j) {
           case 0:
               ui->listWidget_2->addItem(QString::fromStdString(str));
               break;
           case 1:
               ui->listWidget_3->addItem(QString::fromStdString(str));
               break;
           case 2:
               ui->listWidget_4->addItem(QString::fromStdString(str));
               break;
           }
       }
   }

   pReply->deleteLater();
}

void CounterWidget::on_listWidget_doubleClicked(const QModelIndex &index)
{
    ui->label->setText(ui->listWidget->item(index.row())->text());

    for (uint i = 0; i < championinfo.size(); i++) {
        if (ui->label->text().compare(championinfo.at(i).qchampionname) == 0) {
            ui->label_2->setText(championinfo.at(i).qchampiondescription);
            ui->label_3->setPixmap(championinfo.at(i).qchampionpixmap);
            ui->listWidget_2->clear();
            ui->listWidget_3->clear();
            ui->listWidget_4->clear();
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/weak")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/strong")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/good")));
            break;
        }
    }


}

void CounterWidget::on_lineEdit_textChanged(const QString &arg1)
{
    ui->listWidget->clear();
    if (arg1.isEmpty()) {
        for (uint i = 0; i < championinfo.size(); i++) {
            ui->listWidget->addItem(championinfo.at(i).qchampionname);
        }
    }
    else {
        for (uint i = 0; i < championinfo.size(); i++) {
            if (championinfo.at(i).qchampionname.startsWith(arg1, Qt::CaseInsensitive)) {
                ui->listWidget->addItem(championinfo.at(i).qchampionname);
            }
        }
    }
}

void CounterWidget::on_listWidget_2_doubleClicked(const QModelIndex &index)
{
    ui->label->setText(ui->listWidget_2->item(index.row())->text());

    for (uint i = 0; i < championinfo.size(); i++) {
        if (ui->label->text().compare(championinfo.at(i).qchampionname) == 0) {
            ui->label_2->setText(championinfo.at(i).qchampiondescription);
            ui->label_3->setPixmap(championinfo.at(i).qchampionpixmap);
            ui->listWidget_2->clear();
            ui->listWidget_3->clear();
            ui->listWidget_4->clear();
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/weak")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/strong")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/good")));
            break;
        }
    }
}

void CounterWidget::on_listWidget_3_doubleClicked(const QModelIndex &index)
{
    ui->label->setText(ui->listWidget_3->item(index.row())->text());

    for (uint i = 0; i < championinfo.size(); i++) {
        if (ui->label->text().compare(championinfo.at(i).qchampionname) == 0) {
            ui->label_2->setText(championinfo.at(i).qchampiondescription);
            ui->label_3->setPixmap(championinfo.at(i).qchampionpixmap);
            ui->listWidget_2->clear();
            ui->listWidget_3->clear();
            ui->listWidget_4->clear();
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/weak")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/strong")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/good")));
            break;
        }
    }
}

void CounterWidget::on_listWidget_4_doubleClicked(const QModelIndex &index)
{
    ui->label->setText(ui->listWidget_4->item(index.row())->text());

    for (uint i = 0; i < championinfo.size(); i++) {
        if (ui->label->text().compare(championinfo.at(i).qchampionname) == 0) {
            ui->label_2->setText(championinfo.at(i).qchampiondescription);
            ui->label_3->setPixmap(championinfo.at(i).qchampionpixmap);
            ui->listWidget_2->clear();
            ui->listWidget_3->clear();
            ui->listWidget_4->clear();
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/weak")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/strong")));
            pNetworkAccessManager3->get(QNetworkRequest(QUrl("http://www.championselect.net/champions/" + championinfo.at(i).nameToTrimmed() + "/good")));
            break;
        }
    }
}
