#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <memory>
#include <string>
#include "lib/valueobservable.h"
#include "lib/size.h"

class MainViewModel;

typedef std::shared_ptr<MainViewModel> MainViewModelPtr;

class MainViewModel
{
public:
    MainViewModel();
    void initialize();

    using Text = Observable<std::string>;

    Text& text() { return _text; }
    Text& caption() { return _caption; }
    Text& more() { return _more; }
    Observable<Size>& size() { return _size; }

    MainViewModelPtr clone();
private:
    bool _initialized;

    using TextStore = ValueObservable<std::string>;
    TextStore _text;
    TextStore _caption;
    TextStore _more;

    using SizeStore = ValueObservable<Size>;
    SizeStore _size;
};

#endif // MAINVIEWMODEL_H
