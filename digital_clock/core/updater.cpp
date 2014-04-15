#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include "updater.h"

#define S_OPT_LAST_UPDATE        "state/last_update"

namespace digital_clock {
namespace core {

Updater::Updater(QObject* parent)
  : QObject(parent),
    check_beta_(false), autoupdate_(true), update_period_(3),
    force_update_(false), was_error_(false) {
  QSettings settings;
  last_update_ = settings.value(S_OPT_LAST_UPDATE, QDate(2013, 6, 18)).value<QDate>();
  downloader_ = new HttpClient(this);
  connect(downloader_, &HttpClient::ErrorMessage, [=] (const QString& msg) {
    was_error_ = true;
    emit ErrorMessage(msg);
  });
  connect(downloader_, &HttpClient::DataDownloaded,
          [=] (const QByteArray& data) { data_.append(data); });
  connect(downloader_, &HttpClient::finished, this, &Updater::ProcessData);
}

Updater::~Updater() {
  if (downloader_->isRunning()) downloader_->cancel();
  QSettings settings;
  settings.setValue(S_OPT_LAST_UPDATE, last_update_);
}

void Updater::CheckForUpdates(bool force) {
  force_update_ = force;
  was_error_ = false;
  data_.clear();
  downloader_->startRequest(QUrl("http://digitalclock4.sourceforge.net/latest.json"));
}

void Updater::SetCheckForBeta(bool check) {
  check_beta_ = check;
}

void Updater::SetAutoupdate(bool update) {
  autoupdate_ = update;
}

void Updater::SetUpdatePeriod(qint64 period) {
  update_period_ = period;
}

void Updater::TimeoutHandler() {
  if (!autoupdate_ || was_error_ || (downloader_ && downloader_->isRunning())) return;
  if (last_update_.daysTo(QDate::currentDate()) >= update_period_) CheckForUpdates(false);
}

void Updater::ProcessData() {
  if (was_error_) return;
  QJsonParseError err;
  QJsonDocument js_doc = QJsonDocument::fromJson(data_, &err);
  if (err.error != QJsonParseError::NoError) {
    emit ErrorMessage(err.errorString());
    return;
  }
  QJsonObject js_obj = js_doc.object().value("stable").toObject();
  QString latest = js_obj.value("version").toString();
  QDate last_build = QDateTime::fromString(js_obj.value("timestamp").toString(),
                                           Qt::RFC2822Date).toUTC().date();
  QString link = js_obj.value("download").toString();
  if (check_beta_) {
    js_obj = js_doc.object().value("testing").toObject();
    QString t_version = js_obj.value("version").toString("-");
    if (t_version != "-") {
      latest = t_version;
      last_build = QDateTime::fromString(js_obj.value("timestamp").toString(),
                                         Qt::RFC2822Date).toUTC().date();
      link = js_obj.value("download").toString();
    }
  }

  if (latest > QCoreApplication::applicationVersion() ||
      QDateTime::fromString(__TIMESTAMP__).toUTC().date() < last_build) {
    latest = QString("%1, %2").arg(latest).arg(last_build.toString(Qt::SystemLocaleShortDate));
    emit NewVersion(latest, link);
  } else {
    if (force_update_) emit UpToDate();
  }
  last_update_ = QDate::currentDate();
  data_.clear();
}

} // namespace core
} // namespace digital_clock
