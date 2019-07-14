#include "appview.h"

AppView::AppView()
{

}

void AppView::showMain(MainViewModelPtr model)
{
    MainView * view = display<MainViewModel, MainView>(model);
    view->setDelegate(this);
}
