#include "TournamentFileManager.h"
#include <QFile>
#include <QSaveFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <stdexcept>

namespace {
int readScore(const QJsonValue& value) {
    int score = value.toInt(-1);
    if (score < 0) {
        throw std::runtime_error("Invalid score in file.");
    }
    return score;
}
}

void TournamentFileManager::save(const Tournament& tournament, const QString& path) {
    QJsonArray teams;
    for (const Team& team : tournament.getTeams()) {
        teams.append(QString::fromStdString(team.getName()));
    }

    QJsonArray matches;
    for (const Match& match : tournament.getMatches()) {
        QJsonObject entry;
        entry["first"] = QString::fromStdString(match.getFirstTeam());
        entry["second"] = QString::fromStdString(match.getSecondTeam());
        entry["score1"] = match.getFirstScore();
        entry["score2"] = match.getSecondScore();
        matches.append(entry);
    }

    QJsonObject data;
    data["version"] = 1;
    data["teams"] = teams;
    data["matches"] = matches;
    QByteArray bytes = QJsonDocument(data).toJson();

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
        throw std::runtime_error("Could not save file. Check the path and write permission.");
    }
}

void TournamentFileManager::load(Tournament& tournament, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Could not open file. Check the path and read permission.");
    }

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (file.error() != QFileDevice::NoError) {
        throw std::runtime_error("Could not read file.");
    }
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        throw std::runtime_error("Malformed JSON file.");
    }

    QJsonObject data = document.object();
    if (data["version"] != QJsonValue(1) || !data["teams"].isArray() || !data["matches"].isArray()) {
        throw std::runtime_error("Unsupported tournament file structure/version.");
    }

    WinDrawLossRule rule;
    Tournament loaded(rule);
    for (const QJsonValue& team : data["teams"].toArray()) {
        if (!team.isString()) {
            throw std::runtime_error("Invalid team in file.");
        }
        loaded.registerTeam(Team(team.toString().toStdString()));
    }

    for (const QJsonValue& value : data["matches"].toArray()) {
        QJsonObject match = value.toObject();
        if (!value.isObject() || !match["first"].isString() || !match["second"].isString()) {
            throw std::runtime_error("Invalid match in file.");
        }
        std::string first = match["first"].toString().toStdString();
        std::string second = match["second"].toString().toStdString();
        int score1 = readScore(match["score1"]);
        int score2 = readScore(match["score2"]);
        loaded.recordMatch(first, second, score1, score2);
    }

    tournament.replaceData(loaded);
}
