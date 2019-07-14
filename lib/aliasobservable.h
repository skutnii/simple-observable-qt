#ifndef ALIASOBSERVABLE_H
#define ALIASOBSERVABLE_H

#include "observable.h"

/*!
 * \brief An 'virtual' observable
 *        - an alias to some value that may be not representable directly.
 */
template <typename T>
class AliasObservable : public Observable<T>
{
public:
    typedef std::function<T ()> Getter;
    typedef std::function<void (T)> Setter;

    AliasObservable() {}

    using Base = Observable<T>;

    AliasObservable(const Getter& get, const Setter& set, bool firesOnAddCallback = true) :
        Base(firesOnAddCallback), _getter(get), _setter(set) {}

    template<typename G, typename S>
    AliasObservable(G get, S set, bool firesOnAddCallback = true) :
        Base(firesOnAddCallback), _getter(get), _setter(set) {}

    AliasObservable(const AliasObservable& other) :
        AliasObservable(other._getter, other._setter, other.firesOnAddCallback()) {}

    T get()
    {
        return _getter();
    }

protected:

    void doSet(T& value)
    {
        _setter(value);
    }

private:
    Getter _getter;
    Setter _setter;
};

#endif // ALIASOBSERVABLE_H
