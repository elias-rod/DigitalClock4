#include <QSettings>
#include "clock_raster_skin.h"

ClockRasterSkin::ClockRasterSkin(const QDir& skin_root) {
  QSettings config(skin_root.filePath("skin.ini"), QSettings::IniFormat);
  // load info
  info_[SI_NAME] = config.value("info/name", "unknown").toString();
  info_[SI_VERSION] = config.value("info/version", "unknown").toString();
  info_[SI_AUTHOR] = config.value("info/author", "unknown").toString();
  info_[SI_EMAIL] = config.value("info/e-mail", "unknown").toString();
  info_[SI_COMMENT] = config.value("info/comment", "unknown").toString();
  // load image files
  for (int i = 0; i < 10; ++i) {
    QString key = QString::number(i);
    orig_images_[key[0]] = QPixmap(skin_root.filePath(
                                     config.value("files/" + key, "invalid").toString()));
  }
  orig_images_[':'] = QPixmap(skin_root.filePath(config.value("files/s1", "invalid").toString()));
  orig_images_[' '] = QPixmap(skin_root.filePath(config.value("files/s2", "invalid").toString()));
  orig_images_['a'] = QPixmap(skin_root.filePath(config.value("files/am", "invalid").toString()));
  orig_images_['p'] = QPixmap(skin_root.filePath(config.value("files/pm", "invalid").toString()));
  orig_images_['A'] = orig_images_['a'];
  orig_images_['P'] = orig_images_['p'];
}

void ClockRasterSkin::ProcSeparators() {
  for (int i = 0; i < seps_.length(); ++i) {
    if (orig_images_.find(seps_[i]) == orig_images_.end()) {
      orig_images_[seps_[i]] = orig_images_[':'];
    }
  }
}