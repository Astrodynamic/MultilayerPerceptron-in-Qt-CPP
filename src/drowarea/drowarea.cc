#include "drowarea.h"

DrowArea::DrowArea(QObject *parent) : QGraphicsScene(parent) {}

void DrowArea::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    addEllipse(event->scenePos().x() - size_ / 2,
               event->scenePos().y() - size_ / 2, size_, size_, QPen(Qt::NoPen),
               QBrush(brush_color_));
    lastPoint_ = event->scenePos();
    line_end_ = false;
  }
}

void DrowArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (!line_end_) {
    addLine(lastPoint_.x(), lastPoint_.y(), event->scenePos().x(),
            event->scenePos().y(),
            QPen(brush_color_, size_, Qt::SolidLine, Qt::RoundCap));
    lastPoint_ = event->scenePos();
  }
}

void DrowArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    clear();
  } else if (event->button() == Qt::LeftButton) {
    line_end_ = true;
    emit updatePicture();
  }
}

void DrowArea::SetBrushSize(int val) { size_ = val; }

void DrowArea::SetBrushType(int type) {
  switch (type) {
    case 1:
      brush_color_.setNamedColor("white");
      break;
    default:
      brush_color_.setNamedColor("black");
      break;
  }
}

void DrowArea::SetImage() {
  QString file_name(
      QFileDialog::getOpenFileName(nullptr, tr("Open Image"), QDir::homePath(),
                                   tr("Image Files (*.png *.jpg *.bmp)")));
  if (!file_name.isEmpty()) {
    clear();
    QPixmap load_img(file_name);
    addPixmap(load_img.scaled(512, 512, Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation));
    emit updatePicture();
  }
}
