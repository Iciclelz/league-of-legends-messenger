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
#include "statusxml.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#define RegexMatch(pattern, begin, end) QRegularExpressionMatch(QRegularExpression(pattern).match(status)).captured(0).trimmed().replace(begin, "").replace(end, "")

StatusXml::StatusXml(QString status)
{
    this->status = status;

    ProfileIconId = RegexMatch("<profileIcon>.*</profileIcon>", "<profileIcon>", "</profileIcon>");
    Level = RegexMatch("<level>.*</level>", "<level>", "</level>");
    Wins = RegexMatch("<wins>.*</wins>", "<wins>", "</wins>");
    Leaves = RegexMatch("<leaves>.*</leaves>", "<leaves>", "</leaves>");
    Tier = RegexMatch("<tier>.*</tier>", "<tier>", "</tier>");
    ChampionMasteryScore = RegexMatch("<championMasteryScore>.*</championMasteryScore>", "<championMasteryScore>", "</championMasteryScore>");
    RankedLeagueName = RegexMatch("<rankedLeagueName>.*</rankedLeagueName>", "<rankedLeagueName>", "</rankedLeagueName>").replace("&apos;", "'");
    RankedLeagueDivison = RegexMatch("<rankedLeagueDivision>.*</rankedLeagueDivision>", "<rankedLeagueDivision>", "</rankedLeagueDivision>");
    RankedLeagueTier = RegexMatch("<rankedLeagueTier>.*</rankedLeagueTier>", "<rankedLeagueTier>", "</rankedLeagueTier>");
    RankedWins = RegexMatch("<rankedWins>.*</rankedWins>", "<rankedWins>", "</rankedWins>");
    StatusMessage = RegexMatch("<statusMsg>.*</statusMsg>", "<statusMsg>", "</statusMsg>");
}

QString StatusXml::formatted_status()
{
    QString organized_status =  "LEVEL " + Level;

    if (!RankedLeagueTier.isEmpty()) {
        organized_status += "\n" + RankedLeagueTier + " " + RankedLeagueDivison;
    }

    if (!RankedLeagueName.isEmpty()) {
        organized_status += "\n" + RankedLeagueName;
    }

    if (!RankedWins.isEmpty()) {
        organized_status += "\nRANKED WINS: " + RankedWins;
    }

    if (!StatusMessage.isEmpty()) {
        organized_status += "\n\n" + StatusMessage;
    }

    return organized_status;
}
