#include <QApplication>
#include "todo.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ToDo todo;
    todo.show();
    return app.exec();
}