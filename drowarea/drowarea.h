#ifndef DROWAREA_H
#define DROWAREA_H

#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class DrowArea : public QGraphicsScene {
  Q_OBJECT
 public:
  explicit DrowArea(QObject *parent = nullptr);

 private:
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  QPointF m_lastPoint;
  QColor m_brush_color{Qt::black};
  int m_size{45};
  bool m_line_end{true};

 public slots:
  void SetBrushSize(int);
  void SetBrushType(int);
  void SetImage();

 signals:
  void updatePicture();
};

#endif  // DROWAREA_H
