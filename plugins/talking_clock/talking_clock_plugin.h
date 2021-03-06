/*
    Digital Clock: talking clock plugin
    Copyright (C) 2016-2020  Nick Korotysh <nick.korotysh@gmail.com>

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

#ifndef TALKING_CLOCK_TALKING_CLOCK_PLUGIN_H
#define TALKING_CLOCK_TALKING_CLOCK_PLUGIN_H

#include <iclock_plugin.h>
#include <iplugin_init.h>

#include <QTimeZone>

class QTextToSpeech;

namespace talking_clock {

class TalkingClockPlugin : public IClockPlugin, public ISettingsPluginInit
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID CLOCK_PLUGIN_INTERFACE_IID FILE "talking_clock.json")
  Q_INTERFACES(IClockPlugin ISettingsPluginInit)

public:
  TalkingClockPlugin();

  void Init(const QMap<Option, QVariant>& current_settings) override;

public slots:
  void Start() override;
  void Stop() override;

  void Configure() override;

  void SettingsListener(Option option, const QVariant& new_value) override;
  void TimeUpdateListener() override;

private slots:
  void InitSpeechEngine();

private:
  bool started_;
  QTextToSpeech* speech_;
  bool playback_allowed_;
  bool local_time_;
  QTimeZone time_zone_;
};

} // namespace talking_clock

#endif // TALKING_CLOCK_TALKING_CLOCK_PLUGIN_H
