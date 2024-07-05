#include "qkernel.h"
#include <QApplication>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //IMDialog w;
    //w.show();

    QKernel kernel;

    return a.exec();
}
