#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include "mainview.h"
#include <QWidget>

class AppView : public MainView::Delegate
{
public:
    AppView();
    virtual void showMain(MainViewModelPtr model);
private:
    template<typename VM, typename V>
    V* display(std::shared_ptr<VM> pModel);
};

template<typename VM, typename V>
V* AppView::display(std::shared_ptr<VM> pModel)
{
    V* view = new V();
    view->setAttribute(Qt::WA_DeleteOnClose);
    view->setViewModel(pModel);
    view->show();
    return view;
}

#endif // VIEWMANAGER_H
