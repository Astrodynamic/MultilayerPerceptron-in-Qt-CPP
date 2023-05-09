#include "normalaizeimage.h"

NormalaizeImage::NormalaizeImage(QObject* parent) : QObject(parent) {}

QImage NormalaizeImage::normalize(QPixmap input_image, int size) {
  QImage image(input_image.toImage());
  image =
      image.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  return image;
}
