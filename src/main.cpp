#include "Tournament.h"
#include "TournamentFileManager.h"
#include <QtWidgets>
#include <exception>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    WinDrawLossRule drawRule;
    WinLossRule winRule;
    Tournament tournament(drawRule);

    QWidget window;
    window.setWindowTitle("Tournament Scoreboard");
    QVBoxLayout layout(&window);

    QLineEdit name;
    name.setPlaceholderText("Fictional team name");
    QPushButton add("Register team");

    QComboBox first, second, scoring;
    scoring.addItems({"Win / Draw / Loss (3 / 1 / 0)", "Win / Loss (3 / 0; draws earn 0)"});

    QSpinBox score1, score2;
    score1.setRange(0, 1000000); score2.setRange(0, 1000000);
    QPushButton record("Record match");

    QLineEdit path;
    path.setPlaceholderText("Full path to tournament JSON file");
    QPushButton save("Save"), load("Load (replace current tournament)");

    QLabel message;
    message.setWordWrap(true);
    QTableWidget table(0, 5);
    table.setHorizontalHeaderLabels({"Team", "W", "D", "L", "Points"});
    table.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table.setEditTriggers(QAbstractItemView::NoEditTriggers);

    QListWidget history;
    layout.addWidget(new QLabel("1. Register teams"));
    layout.addWidget(&name); layout.addWidget(&add);
    layout.addWidget(new QLabel("2. Record a match"));
    layout.addWidget(&first); layout.addWidget(&score1);
    layout.addWidget(&second); layout.addWidget(&score2); layout.addWidget(&record);
    layout.addWidget(new QLabel("3. Compare standings using a scoring rule"));
    layout.addWidget(&scoring); layout.addWidget(&table);
    layout.addWidget(new QLabel("Match history")); layout.addWidget(&history);
    layout.addWidget(&path); layout.addWidget(&save); layout.addWidget(&load); layout.addWidget(&message);

    auto refresh = [&] {
        auto firstName = first.currentText(), secondName = second.currentText();
        first.clear(); second.clear();
        for (const auto& team : tournament.getTeams()) {
            first.addItem(QString::fromStdString(team.getName())); second.addItem(QString::fromStdString(team.getName()));
        }
        if (first.findText(firstName) >= 0) first.setCurrentText(firstName);
        if (second.findText(secondName) >= 0) second.setCurrentText(secondName);
        table.setRowCount(static_cast<int>(tournament.getTeams().size()));
        int row = 0;
        displayItems(tournament.standings(), [&](const Team& team) {
            QStringList cells{QString::fromStdString(team.getName()), QString::number(team.getWins()),
                QString::number(team.getDraws()), QString::number(team.getLosses()), QString::number(tournament.pointsFor(team))};
            for (int col = 0; col < cells.size(); ++col) table.setItem(row, col, new QTableWidgetItem(cells[col]));
            ++row;
        });
        history.clear();
        displayItems(tournament.getMatches(), [&](const Match& match) {
            history.addItem(QString("%1 %2 : %3 %4").arg(QString::fromStdString(match.getFirstTeam()))
                .arg(match.getFirstScore()).arg(match.getSecondScore()).arg(QString::fromStdString(match.getSecondTeam())));
        });
    };

    auto perform = [&](auto action, const QString& success) {
        try { action(); refresh(); message.setText(success); message.setStyleSheet("color: darkgreen"); }
        catch (const std::exception& error) { message.setText(QString::fromUtf8(error.what())); message.setStyleSheet("color: firebrick"); }
    };
    auto registerTeam = [&] { perform([&] { tournament.registerTeam(name.text().trimmed().toStdString()); name.clear(); }, "Team registered."); };

    QObject::connect(&add, &QPushButton::clicked, registerTeam);
    QObject::connect(&name, &QLineEdit::returnPressed, registerTeam);
    QObject::connect(&record, &QPushButton::clicked, [&] { perform([&] {
        tournament.recordMatch(first.currentText().toStdString(), second.currentText().toStdString(), score1.value(), score2.value());
    }, "Match recorded."); });
    
    QObject::connect(&scoring, &QComboBox::currentIndexChanged, [&](int index) { perform([&] {
        const ScoringRule& selected = index == 0 ? static_cast<const ScoringRule&>(drawRule) : static_cast<const ScoringRule&>(winRule);
        tournament.setRule(selected);
    }, "Standings recalculated."); });

    QObject::connect(&save, &QPushButton::clicked, [&] { perform([&] { 
        TournamentFileManager::save(tournament, path.text()); 
    }, "Saved."); });

    QObject::connect(&load, &QPushButton::clicked, [&] { perform([&] { 
        TournamentFileManager::load(tournament, path.text()); 
    }, "Loaded. Scoring rule selection is unchanged."); });
    
    window.resize(740, 850);
    window.show();

    return app.exec();
}
