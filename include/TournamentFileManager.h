#pragma once
#include "Tournament.h"
#include <QString>

class TournamentFileManager {
public:
    static void save(const Tournament& tournament, const QString& path);
    static void load(Tournament& tournament, const QString& path);
};
