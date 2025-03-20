#include "ui/renderfilterdialog.h"

#include <QColor>

#include "asynclevelloader.h"
#include "level/color.h"
#include "level/map_color_palette.h"
#include "resourcemanager.h"
#include "ui_renderfilterdialog.h"

namespace {

QColor height_to_color(int height, int dim) {
    static int min[]{-64, 0, 0};
    static int max[]{319, 127, 255};
    if (height < min[dim]) height = min[dim];
    if (height > max[dim]) height = max[dim];
    auto gray = static_cast<int>(
        static_cast<qreal>(height - min[dim])
        / static_cast<qreal>(max[dim] - min[dim]) * 255.0
    );
    return {255 - gray, 255 - gray, 255 - gray};
}

} // namespace

RenderFilterDialog::RenderFilterDialog(QWidget* parent)
: QDialog(parent),
  ui(new Ui::RenderFilterDialog) {
    ui->setupUi(this);
    this->setWindowTitle("Configuring Map Filters");
    ui->layer_slider->setSingleStep(1);
    ui->layer_slider->setRange(-63, 319);
}

RenderFilterDialog::~RenderFilterDialog() { delete ui; }

void RenderFilterDialog::fillInUI() {
    // layer
    ui->layer_slider->setValue(this->filter_.layer);
    ui->enable_layer_box->setChecked(this->filter_.enable_layer_);
    ui->block_black_box->setChecked(this->filter_.block_black_mode_);
    ui->biome_black_box->setChecked(this->filter_.biome_black_mode_);
    ui->actor_black_box->setChecked(this->filter_.actor_black_mode_);

    QStringList actor_list;
    for (const auto& actor : this->filter_.actors_list_)
        actor_list << actor.c_str();
    ui->actor_text_edit->setPlainText(actor_list.join(','));

    QStringList block_list;
    for (const auto& block : this->filter_.blocks_list_)
        block_list << block.c_str();
    ui->block_text_edit->setPlainText(block_list.join(','));

    QStringList biome_list;
    for (const auto& biome : this->filter_.biomes_list_)
        biome_list << QString::number(biome);
    ui->biome_text_edit->setPlainText(biome_list.join(','));
}

void RenderFilterDialog::collectFilerData() {
    this->filter_.layer             = ui->layer_slider->value();
    this->filter_.enable_layer_     = ui->enable_layer_box->isChecked();
    this->filter_.block_black_mode_ = ui->block_black_box->isChecked();
    this->filter_.actor_black_mode_ = ui->actor_black_box->isChecked();
    this->filter_.biome_black_mode_ = ui->biome_black_box->isChecked();

    auto blocks = ui->block_text_edit->toPlainText().trimmed().split(",");
    auto biomes = ui->biome_text_edit->toPlainText().trimmed().split(",");
    auto actors = ui->actor_text_edit->toPlainText().trimmed().split(",");

    this->filter_.blocks_list_.clear();
    this->filter_.biomes_list_.clear();
    this->filter_.actors_list_.clear();

    for (const auto& b : blocks) {
        auto s = b.trimmed();
        if (!s.isEmpty()) this->filter_.blocks_list_.insert(s.toStdString());
    }
    for (const auto& b : biomes) {
        auto s = b.trimmed();
        if (!s.isEmpty()) this->filter_.biomes_list_.insert(s.toInt());
    }
    for (const auto& b : actors) {
        auto s = b.trimmed();
        if (!s.isEmpty()) this->filter_.actors_list_.insert(s.toStdString());
    }
}

void RenderFilterDialog::on_current_layer_lineedit_textEdited(
    const QString& arg1
) {
    ui->layer_slider->setValue(ui->current_layer_lineedit->text().toInt());
}

void RenderFilterDialog::on_layer_slider_valueChanged(int value) {
    ui->current_layer_lineedit->setText(QString::number(value));
}


/**
 * Render a block's data based on the search results
 * @param f MapFilter object
 * @param ch block object
 * @param chx x coordinate in the block
 * @param chz z coordinate in the block
 * @param y y coordinate
 * @param rw region coordinate w
 * @param rh region coordinate h
 * @param region region data object
 */
void setRegionBlockData(
    const MapFilter* f,
    bl::chunk*       ch,
    int              chx,
    int              chz,
    int              y,
    int              rw,
    int              rh,
    ChunkRegion*     region
) {
    if (!ch || !f) return;
    const int X     = (rw << 4) + chx;
    const int Z     = (rh << 4) + chz;
    auto      info  = ch->get_block(chx, y, chz);
    auto      biome = ch->get_biome(chx, y, chz);
    auto      raw   = ch->get_block_raw(chx, y, chz);
    auto      color = bl_map_color_palette.get_color(*raw);

    region->terrain_bake_image_.setPixelColor(
        X,
        Z,
        QColor(color.r(), color.g(), color.b(), color.a())
    );

    if ((f->biomes_list_.count(biome) == 0) == f->biome_black_mode_) {
        // Biome filtering (just don't display, no search function)
        auto biome_color = bl::get_biome_color(biome);
        region->biome_bake_image_.setPixelColor(
            X,
            Z,
            QColor(
                biome_color.r(),
                biome_color.g(),
                biome_color.b(),
                biome_color.a()
            )
        );
    }
    region->height_bake_image_
        .setPixelColor(X, Z, height_to_color(y, ch->get_pos().dim));
    // setup tips
    auto& tips      = region->tips_info_[X][Z];
    tips.block_name = info.name;
    tips.biome      = biome;
    tips.height     = static_cast<int16_t>(y);
}

// Terrain, biome rendering and coordinate data settings
void MapFilter::renderImages(bl::chunk* ch, int rw, int rh, ChunkRegion* region)
    const {
    if (!ch || !region) return;
    auto [miny, maxy] = ch->get_pos().get_y_range(ch->get_version());
    if (this->enable_layer_) {
        // Layer Selection Mode
        if (this->layer > maxy || this->layer < miny) return;
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                auto b = ch->get_block_fast(i, this->layer, j);
                if ((this->blocks_list_.count(b.name) == 0)
                    == this->block_black_mode_) {
                    setRegionBlockData(
                        this,
                        ch,
                        i,
                        j,
                        this->layer,
                        rw,
                        rh,
                        region
                    );
                }
            }
        }

    } else {
        // No layers, search for whitelist blocks from top to bottom
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                int  y = ch->get_height(i, j);
                bool found{false};
                while (y >= miny) {
                    auto b = ch->get_block_fast(i, y, j);
                    if ((this->blocks_list_.count(b.name) == 0)
                        == this->block_black_mode_) {
                        found = true;
                        break;
                    }
                    y--;
                }
                if (found) {
                    setRegionBlockData(this, ch, i, j, y, rw, rh, region);
                }
            }
        }
    }
}

void MapFilter::bakeChunkActors(bl::chunk* ch, ChunkRegion* region) const {
    if (!ch) return;
    auto entities = ch->entities();
    for (auto& e : entities) {
        auto key = QString(e->identifier().c_str()).replace("minecraft:", "");
        if ((this->actors_list_.count(key.toStdString()) == 0)
            == this->actor_black_mode_) {
            region->actors_[ActorImage(key)].push_back(e->pos());
        }
    }
}
