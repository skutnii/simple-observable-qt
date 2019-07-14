#ifndef UIBINDING_H
#define UIBINDING_H

#include <QObject>
#include "aliasobservable.h"

/*!
 * \brief An observable that represents a value in the Qt UI,
 *        e.g. a text field's value.
 */
template<typename T>
class UIBinding : public AliasObservable<T>
{
public:
    using Base = AliasObservable<T>;
    using Getter = typename Base::Getter;
    using Setter = typename Base::Setter;

    template<typename G, typename S, typename O, typename A>
    UIBinding(G get, S set, O* sender, void (O::*signal)(A)) :
        Base(get, set),
        _locked(false)
    {
        _connection = QObject::connect(sender, signal,
                                       [this](A value) {
                                           if (!_locked)
                                               this->onChange(value);
                                       });
    }

    virtual void set(T value);

    virtual ~UIBinding()
    {
        QObject::disconnect(_connection);
    }
private:
    QMetaObject::Connection _connection;
    bool _locked;
};

template<typename T>
void UIBinding<T>::set(T value)
{
    if (_locked)
        return;
    else
        _locked = true;

    Base::set(value);

    _locked = false;
}

#endif // UIBINDING_H
