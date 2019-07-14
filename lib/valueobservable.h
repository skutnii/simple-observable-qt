#ifndef VALUEOBSERVABLE_H
#define VALUEOBSERVABLE_H

#include "observable.h"

/*!
 * \brief A simple observable that just stores a single value.
 */
template<typename T>
class ValueObservable : public Observable<T>
{
public:
    ValueObservable(const T& value, bool firesOnAddCallback = true) :
        Observable<T>(firesOnAddCallback), _value(value) {}

    virtual T get();
protected:
    virtual void doSet(T& newValue);

private:
    T _value;
};

template<typename T>
void ValueObservable<T>::doSet(T& newValue)
{
    _value = newValue;
}

template<typename T>
T ValueObservable<T>::get()
{
    return _value;
}

#endif // VALUEOBSERVABLE_H
