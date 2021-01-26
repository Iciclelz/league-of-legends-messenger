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
#ifndef COUNTERWIDGET_H
#define COUNTERWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <vector>

struct ChampionInfo {
    QString qchampionname;
    QString qchampiondescription;
    QString qchampionid;
    QPixmap qchampionpixmap;

    ChampionInfo(QString qchampionname, QString qchampiondescription, QString qchampionid){
        this->qchampionname = qchampionname;
        this->qchampiondescription = qchampiondescription;
        this->qchampionid = qchampionid;
    }

    void setImage(QPixmap pixmap) {
        this->qchampionpixmap = pixmap;
    }

    QString nameToTrimmed() {
        return qchampionname.replace(' ', "").replace("'", "").replace('.', "");
    }
};

namespace Ui {
class CounterWidget;
}

class CounterWidget : public QWidget
{
    Q_OBJECT
private:
    std::vector<ChampionInfo> championinfo;
public:
    explicit CounterWidget(QWidget *parent = 0);
    ~CounterWidget();

    void OnShow();

public slots:
    void GetChampionNetworkReplyFinished(QNetworkReply*);
    void GetChampionImageFinished(QNetworkReply*);
    void GetChampionDetailsFinished(QNetworkReply*);

private slots:
    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_2_doubleClicked(const QModelIndex &index);

    void on_listWidget_3_doubleClicked(const QModelIndex &index);

    void on_listWidget_4_doubleClicked(const QModelIndex &index);

private:
    Ui::CounterWidget *ui;
    QNetworkAccessManager* pNetworkAccessManager;
    QNetworkAccessManager* pNetworkAccessManager2;
    QNetworkAccessManager* pNetworkAccessManager3;
   // QNetworkAccessManager* pChampionDetailsManager;
};

#endif // COUNTERWIDGET_H
