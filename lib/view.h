#ifndef VIEW_H
#define VIEW_H

#include <memory>
#include <QWidget>
#include <QResizeEvent>
#include "size.h"
#include "valueobservable.h"

/*!
 * \brief View mixin template.
 * Adds MVVM capabilities to Qt widgets.
 *
 * The first template parameter is the view model class,
 * the second one is the Qt widget to be subclassed.
 *
 * A view is supposed to do three things:
 *  1) UI layout;
 *  2) data bindings;
 *  3) signal handling.
 */
template<typename VM, typename W>
class View : public W
{
public:
    using ViewModelPtr = std::shared_ptr<VM>;

    View(QWidget *parent = nullptr,
         Qt::WindowFlags flags = Qt::WindowFlags());

    virtual ViewModelPtr viewModel();
    virtual void setViewModel(ViewModelPtr vm);

    Observable<Size>& sizeHandle() { return _size; }
protected:
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void resizeEvent(QResizeEvent *event);
private:
    ViewModelPtr _viewModel;

    using SizeStore = ValueObservable<Size>;
    SizeStore _size;
    SizeStore::CallbackPtr _sizeCallback;
};

template<typename VM, typename W>
View<VM, W>::View(QWidget *parent,
     Qt::WindowFlags flags) :
    W(parent, flags),
    _viewModel(nullptr),
    _size(Size(0, 0))
{
    _size.set(Size(this->size().width(), this->size().height()));
    _sizeCallback = _size.addCallback([this](Size size) { this->resize(size.x, size.y); });
}

template<typename VM, typename W>
void View<VM, W>::resizeEvent(QResizeEvent *event)
{
    W::resizeEvent(event);
    _size.set(Size(event->size().width(), event->size().height()));
}

template<typename VM, typename W>
void View<VM, W>::setViewModel(ViewModelPtr vm)
{
    unbind();
    _viewModel = vm;
    bind();
}

template<typename VM, typename W>
typename View<VM, W>::ViewModelPtr View<VM, W>::viewModel()
{
    return _viewModel;
}

#endif // VIEW_H
