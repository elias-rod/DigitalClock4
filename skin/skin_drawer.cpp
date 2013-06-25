#include <QFile>
#include <QList>
#include <QPainter>
#include "skin_drawer.h"

SkinDrawer::SkinDrawer(QObject *parent)
  : QObject(parent), texture_(8, 8) {
  result_ = 0;
  skin_ = 0;
  zoom_ = 1.0;
  txd_per_elem_ = false;
  preview_mode_ = false;
}

SkinDrawer::~SkinDrawer() {
  delete result_;
  delete skin_;
}

void SkinDrawer::LoadSkin(const QString& skin_root) {
  delete skin_;
  skin_ = CreateSkin(skin_root);
}

void SkinDrawer::SetString(const QString& str) {
  str_ = str;
  Redraw();
}

void SkinDrawer::SetZoom(qreal new_zoom) {
  zoom_ = new_zoom;
  Redraw();
}

void SkinDrawer::SetColor(const QColor& new_color) {
  if (!new_color.isValid()) return;
  texture_.fill(new_color);
  Redraw();
}

void SkinDrawer::SetTexture(const QString& filename) {
  if (!QFile::exists(filename)) return;
  texture_.load(filename);
  Redraw();
}

void SkinDrawer::SetTexturePerElement(bool set) {
  txd_per_elem_ = set;
  Redraw();
}

void SkinDrawer::SetPreviewMode(bool set) {
  preview_mode_ = set;
}

void SkinDrawer::Redraw() {
  QList<QPixmap*> elements;
  // get images for all symbols
  for (auto i = str_.begin(); i != str_.end(); ++i) {
    elements.push_back(skin_->GetImage(*i, zoom_, !preview_mode_));
  }
  // calculate result image width and height
  int result_w = 0;
  int result_h = 0;
  for (auto& elem : elements) {
    result_w += elem->width();
    result_h = qMax(result_h, elem->height());
  }
  // leave some space between images
  int space = 4;
  result_w += space * (str_.length() - 1);

  // create result image
  delete result_;
  result_ = new QPixmap(result_w, result_h);
  QPixmap result_mask(result_->size());

  if (txd_per_elem_) {
    for (auto& elem : elements) {
      //
    }
  } else {
    // create alpha mask from all images
    result_mask.fill(Qt::black);
    QPainter mask_painter(&result_mask);
    int x = 0;
    for (auto& elem : elements) {
      mask_painter.drawPixmap(x, 0, *elem);
      x += elem->width() + space;
    }
    mask_painter.end();
    // draw texture
    QPainter result_painter(result_);
    result_painter.drawTiledPixmap(result_->rect(), texture_);
    result_painter.end();
    // apply alpha mask
  }

  emit DrawingFinished(result_);
}