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
  QPointF lastPoint_;
  QColor brush_color_{Qt::black};
  int size_{45};
  bool line_end_{true};

 public slots:
  void SetBrushSize(int);
  void SetBrushType(int);
  void SetImage();

 signals:
  void updatePicture();
};

#endif  // DROWAREA_H
