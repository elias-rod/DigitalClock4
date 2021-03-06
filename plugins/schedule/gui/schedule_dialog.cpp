/*
    Digital Clock: schedule plugin
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

#include "schedule_dialog.h"
#include "ui_schedule_dialog.h"

#include <QSortFilterProxyModel>

#include "core/daily_tasks_provider.h"

#include "gui/task_edit_dialog.h"

namespace schedule {

ScheduleDialog::ScheduleDialog(QWidget* parent) :
  QDialog(parent),
  ui(new Ui::ScheduleDialog)
{
  ui->setupUi(this);

  tasks_model_ = new DailyTasksProvider(this);
  connect(tasks_model_, &DailyTasksProvider::dataChanged, this, &ScheduleDialog::onTaskDetailsChanged);

  sort_model_ = new QSortFilterProxyModel(this);
  sort_model_->setSourceModel(tasks_model_);

  ui->tasks_view->setModel(sort_model_);
  connect(ui->tasks_view->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &ScheduleDialog::onTasksSelectionChanged);

  ui->tasks_view->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  ui->tasks_view->sortByColumn(0, Qt::AscendingOrder);
}

ScheduleDialog::~ScheduleDialog()
{
  delete ui;
}

void ScheduleDialog::setDates(const QList<QDate>& dates)
{
  QString last_date;
  if (ui->dates_box->count() != 0) last_date = ui->dates_box->currentText();
  ui->dates_box->clear();
  for (auto& dt : qAsConst(dates)) {
    ui->dates_box->addItem(dt.toString(), dt);
  }
  if (ui->dates_box->count() != 0) {
    if (!last_date.isEmpty()) {
      ui->dates_box->setCurrentText(last_date);
    } else {
      ui->dates_box->setCurrentIndex(0);
    }
  }
}

void ScheduleDialog::setTasks(const QList<TaskPtr>& tasks)
{
  tasks_model_->setTasks(tasks);
}

void ScheduleDialog::onTaskDetailsChanged(const QModelIndex& tl, const QModelIndex& br)
{
  QModelIndexList changed_indexes;
  for (int i = tl.row(); i <= br.row(); ++i)
    changed_indexes.append(tasks_model_->index(i, 0));
  for (auto& idx : qAsConst(changed_indexes))
    emit taskEdited(tasks_model_->getTask(idx));
}

void ScheduleDialog::onTasksSelectionChanged(const QItemSelection& selection)
{
  ui->edit_btn->setEnabled(ui->tasks_view->selectionModel()->selectedRows().size() == 1);
  ui->del_btn->setEnabled(!selection.isEmpty());
}

void ScheduleDialog::on_dates_box_currentIndexChanged(int index)
{
  if (index < 0 || index >= ui->dates_box->count()) return;
  emit dateChanged(ui->dates_box->itemData(index).toDate());
}

void ScheduleDialog::on_add_btn_clicked()
{
  TaskEditDialog dlg(this);
  if (ui->dates_box->currentData().isValid())
    dlg.setDate(ui->dates_box->currentData().toDate());
  else
    dlg.setDate(QDate::currentDate());
  dlg.setWindowModality(Qt::WindowModal);
  if (dlg.exec() == QDialog::Accepted) {
    TaskPtr task(new Task());
    task->setDate(dlg.date());
    task->setTime(dlg.time());
    task->setNote(dlg.note());
    task->setNotification(dlg.notification());
    emit taskCreated(task);
    // emit dateChanged() to force update view
    if (task->date() == ui->dates_box->currentData().toDate()) {
      emit dateChanged(task->date());
    }
  }
}

void ScheduleDialog::on_del_btn_clicked()
{
  QModelIndexList selected = ui->tasks_view->selectionModel()->selectedRows();
  if (selected.isEmpty()) return;

  for (auto& index : qAsConst(selected)) {
    TaskPtr task = tasks_model_->getTask(sort_model_->mapToSource(index));
    emit taskDeleted(task);
  }
  // emit dateChanged() to force update view
  emit dateChanged(ui->dates_box->currentData().toDate());
}

void ScheduleDialog::on_edit_btn_clicked()
{
  QModelIndex current_index = sort_model_->mapToSource(ui->tasks_view->currentIndex());
  if (!current_index.isValid()) return;

  TaskPtr task = tasks_model_->getTask(current_index);

  TaskEditDialog dlg(this);
  dlg.setDate(task->date());
  dlg.setTime(task->time());
  dlg.setNote(task->note());
  dlg.setNotification(task->notification());
  dlg.setWindowModality(Qt::WindowModal);

  if (dlg.exec() == QDialog::Accepted) {
    if (dlg.date() != task->date()) {
      emit taskDeleted(task);
      TaskPtr new_task(new Task());
      *new_task = *task;
      new_task->setDate(dlg.date());
      new_task->setTime(dlg.time());
      new_task->setNote(dlg.note());
      new_task->setNotification(dlg.notification());
      emit taskCreated(new_task);
      // emit dateChanged() to force update view
      emit dateChanged(ui->dates_box->currentData().toDate());
    } else {
      task->setTime(dlg.time());
      task->setNote(dlg.note());
      task->setNotification(dlg.notification());
      emit taskEdited(task);
    }
  }
}

} // namespace schedule
