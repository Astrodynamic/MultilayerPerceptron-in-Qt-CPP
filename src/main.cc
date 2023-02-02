#include <QApplication>
#include <clocale>

#include "controller.h"

int main(int argc, char **argv) {
  std::setlocale(LC_ALL, "С");

  QApplication app(argc, argv);

  Controller *window = Controller::create();
  window->show();

  int status = app.exec();

  window->destroy();

  return status;
}
