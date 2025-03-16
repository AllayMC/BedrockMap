#pragma once

#include "level/bedrock_key.h"
#include "level/palette.h"

#include <QImage>

void initResources();

QImage* ActorImage(const QString& key);

QImage* OtherNBTIcon();

QImage* PlayerNBTIcon();

QImage* TagIcon(bl::palette::tag_type t);

QImage* VillageNBTIcon(bl::village_key::key_type t);

QImage* BlockActorNBTIcon(const QString& key);

QImage* EntityNBTIcon(const QString& key);

// New APIs vvv

struct IconManager {
    IconManager() = delete;

    static void init();
};
