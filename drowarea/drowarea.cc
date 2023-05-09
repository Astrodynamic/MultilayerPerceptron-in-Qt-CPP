#include "drowarea.h"

DrowArea::DrowArea(QObject *parent) : QGraphicsScene(parent) {}

void DrowArea::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    addEllipse(event->scenePos().x() - m_size / 2,
               event->scenePos().y() - m_size / 2, m_size, m_size, QPen(Qt::NoPen),
               QBrush(m_brush_color));
    m_lastPoint = event->scenePos();
    m_line_end = false;
  }
}

void DrowArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (!m_line_end) {
    addLine(m_lastPoint.x(), m_lastPoint.y(), event->scenePos().x(),
            event->scenePos().y(),
            QPen(m_brush_color, m_size, Qt::SolidLine, Qt::RoundCap));
    m_lastPoint = event->scenePos();
  }
}

void DrowArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    clear();
  } else if (event->button() == Qt::LeftButton) {
    m_line_end = true;
    emit updatePicture();
  }
}

void DrowArea::SetBrushSize(int val) { m_size = val; }

void DrowArea::SetBrushType(int type) {
  switch (type) {
    case 1:
      m_brush_color.setNamedColor("white");
      break;
    default:
      m_brush_color.setNamedColor("black");
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
