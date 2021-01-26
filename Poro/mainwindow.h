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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTreeWidgetItem>
#include "counterwidget.h"
#include "xmppclient.h"

#include <vector>
#include <utility>

//class XmppClient;


struct MessengerTabStruct {
    QString name;
    QString status;
    QWidget *tabpage;
    QLineEdit *lineEdit;
    QTextEdit *textEdit;
    QLayout *layout;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, XmppClient
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent*);

private slots:
    void on_actionAbout_Poro_Client_triggered();

    void on_actionLeague_of_Legends_Counter_triggered();

    void on_friendButton_clicked();

    void on_loginButton_clicked();

    void on_treeWidget_doubleClicked(const QModelIndex &index);

    void OnQtCloseTab(int index);

    void on_actionLogout_triggered();

    void on_enter_key_pressed();

    void on_messengerTabWidget_currentChanged(int index);

    void on_messageQueueTimer_tick();

    void on_comboBox_currentIndexChanged(int index);
    void on_lineEdit_editingFinished();

    void on_actionFriend_List_triggered();

    void on_actionMessenger_triggered();

    void on_actionSet_Riot_API_Key_triggered();

private:
    Ui::MainWindow *ui;
    QPushButton *friendButton;
    QTimer *messageQueueTimer;

private:
    CounterWidget *counterWidget;

public:
    virtual void handleMessage(const Message& stanza, MessageSession* session = 0);

    virtual void handlePresence(const Presence& presence);

    virtual void handleRoster (const Roster &roster);

public:
    std::vector<MessengerTabStruct> openedChats;
    std::vector<std::pair<QString, QString>> messageQueue;

private:
    bool CanAddMessengerTab(QString name);
    MessengerTabStruct CreateMessengerTab(QString name);
    void RemoveMessengerTab(MessengerTabStruct chat);

    MessengerTabStruct GetMessengerTab(QString summonerName);
    MessengerTabStruct *GetMessengerTabPtr(QString summonerName);
    MessengerTabStruct GetMessengerTab(QWidget *tabpage);
    MessengerTabStruct GetMessengerTab(QLineEdit *lineEdit);

    QTreeWidgetItem *GetTreeWidgetItem(QString name);
    void RemoveTreeWidgetItem(QTreeWidgetItem *item);
    void AddTreeWidgetItem(QTreeWidgetItem *item, int primaryNodeIndex);

    int GetMessengerTabIndex(QWidget *tabPage);
    void SetUnreadIndication(QWidget *tabPage);
    void SetUnreadIndication(int index);
    void RemoveUnreadIndication(QWidget *tabPage);
    void RemoveUnreadIndication(int index);
    bool HasUnreadIndication(QWidget *tabPage);

    void OnGetMessage(QString summonerName, QString message);
    QString GetTime();

    void ChangePresence();
};

#endif // MAINWINDOW_H
