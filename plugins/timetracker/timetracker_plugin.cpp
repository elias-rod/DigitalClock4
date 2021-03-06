/*
    Digital Clock: timetracker plugin
    Copyright (C) 2018-2020  Nick Korotysh <nick.korotysh@gmail.com>

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
#include "timetracker_plugin.h"

#include <QCoreApplication>
#include <QGridLayout>

#include "plugin_settings.h"
#include "widget_plugin_settings.h"

#include "timetracker.h"
#include "tracker_widget.h"

namespace timetracker {

// THIS IS A TRICK!
// these variables exist just to survive after plugins
// reload on settings changes (it happens in any case)
// but plugin state is reset after application restart
static const char* const PROP_STATE_ELAPSED = "dcp_timetracker_state_last_elapsed";
static const char* const PROP_STATE_ACTIVE = "dcp_timetracker_state_last_active";

TimetrackerPlugin::TimetrackerPlugin() : tracker_(nullptr)
{
//  InitTranslator(QLatin1String(":/timetracker/timetracker_"));
  info_.display_name = tr("Timetracker");
  info_.description = tr("Very simple timetracker.\n"
                         "Single click to start/pause.\n"
                         "Double click to reset/restart.");
//  InitIcon(":/timetracker/icon.svg.p");
}

void TimetrackerPlugin::Start()
{
  if (tracker_) return;
  tracker_ = new Timetracker();
  // TRICK! restore state from dynamic app properties
  QVariant prop_var = QCoreApplication::instance()->property(PROP_STATE_ELAPSED);
  if (prop_var.isValid()) tracker_->setElapsed(prop_var.toInt());
  prop_var = QCoreApplication::instance()->property(PROP_STATE_ACTIVE);
  if (prop_var.isValid() && prop_var.toBool()) tracker_->start();
  // state is restored before widget initialization to prevent flickering
  ::plugin::WidgetPluginBase::Start();
  settings_->SetOption(::plugin::OptionKey(::plugin::OPT_USE_CLOCK_SKIN), true);
}

void TimetrackerPlugin::Stop()
{
  ::plugin::WidgetPluginBase::Stop();
  if (tracker_) {
    // TRICK! save state to dynamic app properties
    QCoreApplication::instance()->setProperty(PROP_STATE_ACTIVE, tracker_->isActive());
    QCoreApplication::instance()->setProperty(PROP_STATE_ELAPSED, tracker_->elapsed());
    tracker_->stop();
  }
  delete tracker_;
  tracker_ = nullptr;
}

QWidget* TimetrackerPlugin::InitWidget(QGridLayout* layout)
{
  TrackerWidget* w = new TrackerWidget();
  layout->addWidget(w, layout->rowCount(), 0);
  connect(w, &TrackerWidget::clicked, this, &TimetrackerPlugin::onWidgetClicked);
  connect(w, &TrackerWidget::doubleClicked, tracker_, &Timetracker::reset);
  return w;
}

void TimetrackerPlugin::DisplayImage(QWidget* widget, const QImage& image)
{
  static_cast<TrackerWidget*>(widget)->setPixmap(QPixmap::fromImage(image));
}

QString TimetrackerPlugin::GetWidgetText()
{
  int s = tracker_->elapsed();
  int h = s / 3600;
  s -= h * 3600;
  int m = s / 60;
  s -= m * 60;
  return QString("%1:%2:%3").arg(h).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

void TimetrackerPlugin::onWidgetClicked()
{
//  using namespace ::plugin;
  if (tracker_->isActive()) {
    tracker_->stop();
//    settings_->SetOption(OptionKey(OPT_USE_CUSTOM_COLOR), true);
  } else {
    tracker_->start();
//    settings_->SetOption(OptionKey(OPT_USE_CUSTOM_COLOR), false);
  }
}

} // namespace timetracker
