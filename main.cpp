#include "mainview.h"
#include "mainviewmodel.h"
#include <QApplication>
#include "appview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppView appView;

    MainViewModelPtr vm(std::make_shared<MainViewModel>());
    vm->initialize();
    appView.showMain(vm);

    return a.exec();
}
