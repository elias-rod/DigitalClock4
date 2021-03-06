/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2013-2020  Nick Korotysh <nick.korotysh@gmail.com>

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

#include "clock_base_skin.h"

namespace digital_clock {
namespace core {

const BaseSkin::TSkinInfo& BaseSkin::GetInfo() const
{
  return info_;
}

void BaseSkin::SetSeparators(const QString& seps)
{
  seps_ = seps;
  seps_.remove(' ');
  seps_ += ":./";
  QString tmp = seps_;
  seps_.clear();
  for (int i = 0; i < tmp.size(); ++i)
    if (!seps_.contains(tmp[i])) seps_ += tmp[i];
  ProcSeparators();
}

} // namespace core
} // namespace digital_clock
