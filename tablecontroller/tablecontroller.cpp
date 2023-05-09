#include "tablecontroller.h"

TableController::TableController(QWidget* parent) : QTableWidget{parent} {}

void TableController::SendLayouts() {
  QVector<unsigned> depth_vec;
  for (int i = 0; i < rowCount(); ++i) {
    QSpinBox* sbx = qobject_cast<QSpinBox*>(cellWidget(i, 1));
    depth_vec.push_back(sbx->value());
  }
  emit LayoutsSended(depth_vec);
}

void TableController::AddDefLayout() {
  insertRow(0);
  QTableWidgetItem* item = new QTableWidgetItem("Input Layout");
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  setItem(0, 0, item);
  item = new QTableWidgetItem("Max Image resolution");
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  setItem(0, 2, item);
  QSpinBox* spb = new QSpinBox(this);
  spb->setMinimum(1);
  spb->setMaximum(1000);
  spb->setValue(784);
  connect(spb, SIGNAL(valueChanged(int)), this, SLOT(QuadChange(int)));
  setCellWidget(0, 1, spb);

  insertRow(1);
  item = new QTableWidgetItem("Output Layout");
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  setItem(1, 0, item);
  item = new QTableWidgetItem("Count of answers");
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  setItem(1, 2, item);
  spb = new QSpinBox(this);
  spb->setMinimum(1);
  spb->setValue(26);
  setCellWidget(1, 1, spb);

  AddLayout();
  AddLayout();
  qobject_cast<QSpinBox*>(cellWidget(1, 1))->setValue(256);
  qobject_cast<QSpinBox*>(cellWidget(2, 1))->setValue(64);
}

void TableController::QuadChange(int val) {
  QSpinBox* send = (QSpinBox*)sender();
  int sq_val = static_cast<int>(qSqrt(val));
  if (qPow(sq_val, 2) == val - 1) sq_val += 1;
  send->setValue(qPow(sq_val, 2));
}

void TableController::AddLayout() {
  int row_num = rowCount() - 1;
  AddLayout(row_num);
}

void TableController::AddLayout(int position) {
  insertRow(position);
  QSpinBox* spb = new QSpinBox(this);
  spb->setMinimum(1);
  spb->setMaximum(1000);
  spb->setValue(GetDiffVall(position));
  setCellWidget(position, 1, spb);
  QPushButton* remove_btn = new QPushButton(this);
  remove_btn->setText("Remove");
  connect(remove_btn, SIGNAL(clicked()), this, SLOT(OnRemoveBtnCliced()));
  setCellWidget(position, 2, remove_btn);
}

void TableController::RemoveLayout() {
  int row_num = rowCount() - 2;
  if (row_num > 0) {
    RemoveLayout(row_num);
  }
}

void TableController::RemoveLayout(int row) { removeRow(row); }

int TableController::GetDiffVall(int position) {
  int result = qobject_cast<QSpinBox*>(cellWidget(position + 1, 1))->value();
  result += qobject_cast<QSpinBox*>(cellWidget(position - 1, 1))->value();
  return result / 2;
}

void TableController::SetLayouts(QVector<unsigned>& new_layouts) {
  emit SetCount(new_layouts.size() - 2);
}

void TableController::OnRemoveBtnCliced() {
  QPushButton* btn = qobject_cast<QPushButton*>(sender());
  if (btn && rowCount() > 4) {
    QModelIndex index = indexAt(btn->pos());
    RemoveLayout(index.row());
  }
}

void TableController::SetLayots(int value) {
  int row_change = rowCount() - value - 2;
  while (row_change) {
    if (row_change < 0) {
      AddLayout();
      ++row_change;
    } else {
      RemoveLayout();
      --row_change;
    }
  }
}
