#include "mainviewmodel.h"

MainViewModel::MainViewModel() :
    _initialized(false),
    _text(""),
    _caption(""),
    _more(""),
    _size(Size(0, 0))
{
}

void MainViewModel::initialize()
{
    if (_initialized)
        return;

    std::string info("This is an observable Qt example.");
    _caption.set(info);

    std::string text("The values are synchronized.");
    _text.set(text);

    std::string more("More...");
    _more.set(more);

    _size.set(Size(400, 150));

    _initialized = true;
}

MainViewModelPtr MainViewModel::clone()
{
    MainViewModelPtr copy =
            std::make_shared<MainViewModel>();

    copy->text().set(text().get());
    copy->caption().set(caption().get());
    copy->more().set(more().get());
    copy->size().set(size().get());

    return copy;
}
