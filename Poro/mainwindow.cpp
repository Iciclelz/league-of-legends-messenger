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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QTextEdit>
#include <QTimer>
#include <QTooltip>
#include <QColor>
#include <QSettings>
#include <QInputDialog>
#include <QScrollBar>
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dockWidget->setVisible(false);

    friendButton = new QPushButton(this);
    friendButton->setFlat(true);
    friendButton->setText("Friend");
    connect(friendButton, SIGNAL(clicked()), this, SLOT(on_friendButton_clicked()));
    ui->statusBar->addPermanentWidget(friendButton);
    counterWidget = new CounterWidget;

    for (int i = 0; i < 4; i++)
        ui->treeWidget->addTopLevelItem(new QTreeWidgetItem());

    ui->treeWidget->topLevelItem(0)->setText(0, "Available");
    ui->treeWidget->topLevelItem(1)->setText(0, "Away");
    ui->treeWidget->topLevelItem(2)->setText(0, "In Game");
    ui->treeWidget->topLevelItem(3)->setText(0, "Offline");

    ui->comboBox->addItems(QStringList(QList<QString>({"Available", "Away", "In Game", "Offline"})));

    connect(ui->messengerTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(OnQtCloseTab(int)));

    messageQueueTimer = new QTimer(this);
    connect(messageQueueTimer, SIGNAL(timeout()), this, SLOT(on_messageQueueTimer_tick()));
    messageQueueTimer->start(1);

    QSettings registry("Ryukuo", "Poro");
    ui->editUsername->setText(registry.value("Username").toByteArray());
    ui->editPassword->setText(registry.value("Password").toByteArray());
    ui->regionComboBox->setCurrentText(QString(registry.value("Server").toByteArray()));

}

MainWindow::~MainWindow()
{
    delete counterWidget;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *) {
    qApp->quit();
}

void MainWindow::on_actionAbout_Poro_Client_triggered()
{
    QMessageBox::about(this, "About Poro Client", "Poro Client Version 0.0.1.0\n\nCreated by Ryukuo");
}

void MainWindow::on_actionLeague_of_Legends_Counter_triggered()
{
    counterWidget->show();
    counterWidget->OnShow();
}

void MainWindow::on_friendButton_clicked()
{
    ui->dockWidget->setVisible(!ui->dockWidget->isVisible());
    ui->dockWidget->raise();
}

void MainWindow::on_loginButton_clicked()
{
    ui->label->setText(ui->editUsername->text());
    setRegion(ui->regionComboBox->currentIndex());
    Connect(ui->editUsername->text().toUtf8().constData(), ui->editPassword->text().toUtf8().constData());

    QSettings registry("Ryukuo", "Poro");
    registry.setValue("Username", ui->editUsername->text());
    registry.setValue("Password", ui->editPassword->text());
    registry.setValue("Server", ui->regionComboBox->currentText());
    ui->loginFrame->setVisible(false);
}

void MainWindow::handleMessage(const Message &stanza, MessageSession *session)
{
    if (QString::fromStdString(stanza.body()).trimmed().compare("") == 0)
        return;

    messageQueue.push_back(std::make_pair(getSummonerName(QString::fromStdString(stanza.from().username())), QString::fromStdString(stanza.body())));
}


void MainWindow::OnGetMessage(QString summonerName, QString message)
{
    MessengerTabStruct chat;
    if (CanAddMessengerTab(summonerName)) {
        chat = CreateMessengerTab(summonerName);
        ui->messengerTabWidget->addTab(chat.tabpage, chat.name);
    }
    else {
        chat = GetMessengerTab(summonerName);
    }
    if (&chat) {
        chat.textEdit->setTextColor(QColor(Qt::magenta));
        chat.textEdit->append(GetTime() + summonerName + ": " + message);
        chat.textEdit->verticalScrollBar()->setValue(chat.textEdit->verticalScrollBar()->maximum());

        if (ui->messengerTabWidget->currentWidget() != chat.tabpage) {
            SetUnreadIndication(chat.tabpage);
        }
    }
}

QString MainWindow::GetTime()
{
    QTime time = QTime::currentTime();

    QString minute(QString::number(time.minute()));
    if (minute.length() == 1) {
        minute = "0" + minute;
    }

    return QString("[" + QString::number(time.hour()) + ":" + minute  + "] ");
}

void MainWindow::ChangePresence()
{
    if (!isConnected) {
        return;
    }
    Presence::PresenceType type;
    switch (ui->comboBox->currentIndex()) {
    case 0:
        type = Presence::Chat;
        break;
    case 1:
        type = Presence::Away;
        break;
    case 2:
        type = Presence::DND;
        break;
    case 3:
        type = Presence::Unavailable;
        break;

    }

    client->setPresence (type, 1, MakeStatus(1, ui->lineEdit->text().toUtf8().constData(), 0, 0, 0, "", 0, 0, 0, "", "", "", 0));
}

void MainWindow::handlePresence(const Presence &presence)
{
    QString summonerName = getSummonerName(QString::fromStdString(presence.from().username()));
    QTreeWidgetItem *item = GetTreeWidgetItem(summonerName);
    if (item) {
        RemoveTreeWidgetItem(item);

        switch (presence.presence()){
            case Presence::Chat:
                AddTreeWidgetItem(item, 0);
                break;
            case Presence::Away:
                AddTreeWidgetItem(item, 1);
                break;
            case Presence::DND:
                AddTreeWidgetItem(item, 2);
                break;
            case Presence::Unavailable:
                AddTreeWidgetItem(item, 3);
                break;

        }

        if (presence.presence() == Presence::Chat ||
                presence.presence() == Presence::Away ||
                presence.presence() == Presence::DND) {

            item->setToolTip(0, ReformatStatus(QString::fromStdString(presence.status())));
        }
    }




}

void MainWindow::handleRoster(const Roster &roster)
{
    for(auto const &rosteritem : roster) {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, QString::fromStdString(rosteritem.second->name()));
        ui->treeWidget->topLevelItem(3)->addChild(item);
    }
}

bool MainWindow::CanAddMessengerTab(QString name)
{
    for (size_t i = 0; i < openedChats.size(); ++i) {
        if (openedChats.at(i).name.compare(name) == 0) {
            return false;
        }
    }
    return true;
}

MessengerTabStruct MainWindow::CreateMessengerTab(QString name)
{
    MessengerTabStruct chat;
    chat.name = name;

    chat.tabpage = new QWidget;
    chat.layout = new QGridLayout;
    chat.lineEdit = new QLineEdit;
    chat.textEdit = new QTextEdit;

    chat.textEdit->setReadOnly(true);

    chat.layout->setMargin(0);
    chat.layout->addWidget(chat.textEdit);
    chat.layout->addWidget(chat.lineEdit);

    chat.tabpage->setLayout(chat.layout);

    connect(chat.lineEdit, SIGNAL(returnPressed()), this, SLOT(on_enter_key_pressed()));

    openedChats.push_back(chat);

    return chat;
}

void MainWindow::RemoveMessengerTab(MessengerTabStruct chat)
{
     for (size_t i = 0; i < openedChats.size(); ++i) {
            if (openedChats.at(i).name.compare(chat.name) == 0) {
                openedChats.erase(openedChats.begin() + i);
                break;
            }
     }
}

MessengerTabStruct MainWindow::GetMessengerTab(QString summonerName)
{
    for (size_t i = 0; i < openedChats.size(); i++) {
            if (openedChats.at(i).name.compare(summonerName) == 0) {
                return openedChats.at(i);
            }
        }
    return MessengerTabStruct();
}

MessengerTabStruct *MainWindow::GetMessengerTabPtr(QString summonerName)
{
    for (size_t i = 0; i < openedChats.size(); i++) {
            if (openedChats.at(i).name.replace("*", "").compare(summonerName) == 0) {
                return &openedChats.at(i);
            }
        }
    return nullptr;
}

MessengerTabStruct MainWindow::GetMessengerTab(QWidget *tabpage)
{
    for (size_t i = 0; i < openedChats.size(); i++) {
            if (openedChats.at(i).tabpage == tabpage) {
                return openedChats.at(i);
            }
        }
    return MessengerTabStruct();
}

MessengerTabStruct MainWindow::GetMessengerTab(QLineEdit *lineEdit)
{
    for (size_t i = 0; i < openedChats.size(); i++) {
            if (openedChats.at(i).lineEdit == lineEdit) {
                return openedChats.at(i);
            }
        }
    return MessengerTabStruct();
}

QTreeWidgetItem *MainWindow::GetTreeWidgetItem(QString name)
{
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
        for (int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++) {
            if (!ui->treeWidget->topLevelItem(i)->child(j)->text(0).compare(name)) {
                return ui->treeWidget->topLevelItem(i)->child(j);
            }
        }
    }
    return nullptr;
}

void MainWindow::RemoveTreeWidgetItem(QTreeWidgetItem *item)
{
    item->parent()->removeChild(item);
}

void MainWindow::AddTreeWidgetItem(QTreeWidgetItem *item, int primaryNodeIndex)
{
    ui->treeWidget->topLevelItem(primaryNodeIndex)->addChild(item);
}

int MainWindow::GetMessengerTabIndex(QWidget *tabPage)
{
    return ui->messengerTabWidget->indexOf(tabPage);
}

void MainWindow::SetUnreadIndication(QWidget *tabPage)
{
    SetUnreadIndication(GetMessengerTabIndex(tabPage));
}

void MainWindow::SetUnreadIndication(int index)
{
    if (!ui->messengerTabWidget->tabText(index).contains("*")) {
        ui->messengerTabWidget->setTabText(index, ui->messengerTabWidget->tabText(index) + "*");
    }
}

void MainWindow::RemoveUnreadIndication(QWidget *tabPage)
{
    RemoveUnreadIndication(GetMessengerTabIndex(tabPage));
}

void MainWindow::RemoveUnreadIndication(int index)
{
    if (ui->messengerTabWidget->tabText(index).contains("*")) {
            ui->messengerTabWidget->setTabText(index, ui->messengerTabWidget->tabText(index).replace("*", ""));
    }
}

bool MainWindow::HasUnreadIndication(QWidget *tabPage)
{
    return ui->messengerTabWidget->tabText(GetMessengerTabIndex(tabPage)).contains("*");
}


void MainWindow::on_treeWidget_doubleClicked(const QModelIndex &index)
{
    if (ui->treeWidget->currentItem()->parent() == nullptr) {
        return;
    }
    if (CanAddMessengerTab(ui->treeWidget->currentItem()->text(0))) {
        MessengerTabStruct chat = CreateMessengerTab(ui->treeWidget->currentItem()->text(0));
        ui->messengerTabWidget->addTab(chat.tabpage, chat.name);
    }
}

void MainWindow::OnQtCloseTab(int index) {
    QWidget *tabpage = reinterpret_cast<QTabWidget*>(sender())->widget(index);
    RemoveMessengerTab(GetMessengerTab(tabpage));
    delete tabpage;
}

void MainWindow::on_actionLogout_triggered()
{
    ui->loginFrame->setVisible(true);
}

void MainWindow::on_enter_key_pressed()
{
    QLineEdit *lineEdit = reinterpret_cast<QLineEdit*>(sender());

    if (lineEdit->text().trimmed().compare("")) {
        MessengerTabStruct chat = GetMessengerTab(lineEdit);
        client->send(Message(Message::MessageType::Chat, JID((QString("sum" + getSummonerId(chat.name) + "@pvp.net/xiff")).toStdString()), lineEdit->text().toStdString()));

        chat.textEdit->setTextColor(QColor(Qt::green));
        chat.textEdit->append(GetTime() + ui->label->text() + ": " + lineEdit->text());
        chat.textEdit->verticalScrollBar()->setValue(chat.textEdit->verticalScrollBar()->maximum());
        lineEdit->setText("");
    }


}

void MainWindow::on_messengerTabWidget_currentChanged(int index)
{
    if (HasUnreadIndication(ui->messengerTabWidget->widget(index))) {
        RemoveUnreadIndication(index);
    }
}

void MainWindow::on_messageQueueTimer_tick()
{
    if (!messageQueue.empty()) {
        for (int i = 0; i < messageQueue.size();) {
            OnGetMessage(messageQueue.at(i).first, messageQueue.at(i).second);
            messageQueue.erase(messageQueue.begin() + i);
        }
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if (isConnected) {
        ChangePresence();
    }
}

void MainWindow::on_lineEdit_editingFinished()
{
    if (isConnected) {
        ChangePresence();
    }
}

void MainWindow::on_actionFriend_List_triggered()
{
    ui->dockWidget->show();
}

void MainWindow::on_actionMessenger_triggered()
{
    ui->dockWidget_2->show();
}


void MainWindow::on_actionSet_Riot_API_Key_triggered()
{
    SetApiKey (QInputDialog::getText(0, "Poro", "Enter your API Key: ").toUtf8().constData());
}
