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
#include <QApplication>
#include <QStyleFactory>

#pragma comment (lib, "gloox.lib")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QPalette p;
    p = qApp->palette();
    p.setColor(QPalette::Window, QColor(48,48,48));
    p.setColor(QPalette::Button, QColor(48,48,48));
    p.setColor(QPalette::ButtonText, QColor(255,255,255));
    p.setColor(QPalette::WindowText, QColor(255,255,255));
    p.setColor(QPalette::Base, QColor(35,35,35));
    p.setColor(QPalette::Text, QColor(255,255,255));
    p.setColor(QPalette::Highlight, QColor(80,80,80));
    qApp->setPalette(p);

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
