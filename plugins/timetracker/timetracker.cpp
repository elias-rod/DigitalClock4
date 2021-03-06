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
#include "timetracker.h"

namespace timetracker {

Timetracker::Timetracker(QObject* parent) : QObject(parent)
{
}

int Timetracker::elapsed() const
{
  int elapsed = last_elapsed_;
  if (timer_.isValid())
    elapsed += timer_.elapsed() / 1000;
  return elapsed;
}

bool Timetracker::isActive() const
{
  return timer_.isValid();
}

void Timetracker::start()
{
  if (timer_.isValid())
    return;

  timer_.start();
}

void Timetracker::stop()
{
  if (!timer_.isValid())
    return;

  last_elapsed_ = timer_.elapsed() / 1000;
  timer_.invalidate();
}

void Timetracker::reset()
{
  last_elapsed_ = 0;
  if (timer_.isValid())
    timer_.restart();
}

void Timetracker::setElapsed(int elapsed_secs)
{
  last_elapsed_ = elapsed_secs;
}

} // namespace timetracker
