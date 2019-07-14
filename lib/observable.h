#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <functional>
#include <memory>
#include <deque>
#include <algorithm>
#include <unordered_set>

/*!
 * \brief An abstract typed bindable observable value.
 *        The value type must be assignable and comparable.
 */
template<typename T> class Observable
{
public:
    /*!
     * \brief Constructor
     * \param firesOnAddCallback controls
     *        whether the observable executes a callback immediately upon adding.
     */
    Observable(bool firesOnAddCallback = true)
        : _callbacks(), _fireOnAdd(firesOnAddCallback), _bindings() {}

    /*!< firesOnAddCallback property accessors. */
    bool firesOnAddCallback() const { return _fireOnAdd; }
    void setFiresOnAddCallback(bool flag) { _fireOnAdd = flag; }

    /*!< Get the observable's value. */
    virtual T get() = 0;

    /*!< Set a new value. After assignment, the observer fires its callbacks. */
    virtual void set(T value);

    using Callback = std::function<void (T newV)>;
    using CallbackPtr = std::shared_ptr<Callback>;

    /*!
     * \brief Add a callback to be executed after a new value was set.
     *        The returned value is a callback control handle.
     *        It may be used to remove the callback from the observable's list.
     *        The observable keeps a weak reference to the callback only,
     *        so the callback is automatically destroyed,
     *        when the last copy of the control handle is destroyed.
     *        Store the control handle as e.g. a class member
     *        to keep receiving notifications from the observable.
     */
    template<typename F>
    CallbackPtr addCallback(F block)
    {
        CallbackPtr onUpdate(new Callback(block));
        _callbacks.emplace_back(onUpdate);

        if (_fireOnAdd)
        {
            T value = get();
            onUpdate->operator()(value);
        }

        return onUpdate;
    }

    /*!
     * \brief Removes a callback from the observable's list.
     *        Does nothing if the callback does not belong to the list.
     */
    void removeCallback(CallbackPtr callback);

    using Binding = std::shared_ptr<void>;
    using BindingHandle = std::weak_ptr<void>;

    /*!
     * \brief Binds the observable to another one,
     *        so that the callee's value becomes synchronized with the other one's.
     *        If the callee is already bound to some observable, that binding is replaced.
     *
     * \param other - the observable to bind to.
     * \param convert - a functor that transforms other observable's value to the callee's type.
     *
     * \return - a handler to the newly established binding
     */
    template<typename O, typename C>
    BindingHandle bind(Observable<O>& other, C convert)
    {
        Binding binding = other.addCallback(
                    [this, convert](O value)
                    {
                          this->set(convert(value));
                    });

        _bindings.insert(binding);
        return binding;
    }

    /*!
     * \brief Remove all the observable bindings, if any.
     */
    void unbind()
    {
        _bindings.clear();
    }

    /*!
     * \brief Forcibly add a binding to the observable's binding list.
     *        Mostly useful to the observable's subclasses.
     */
    BindingHandle addBinding(Binding binding)
    {
        _bindings.insert(binding);
        return binding;
    }

    /*!
     * \brief Removes a binding by its handle.
     *        If the binding is not present in the list,
     *        or the handle has expired, does nothing.
     */
    void removeBinding(BindingHandle h)
    {
        Binding b = h.lock();
        if (b && (_bindings.find(b) != _bindings.end()))
            _bindings.erase(b);
    }

    /*!
     * \brief Create a two way binding between observables,
     *        so that the values become synchronized.
     * \param other - another observable to bind to.
     * \param convert -  a functor that will be used to convert
     *                   the other observable's value to the callee's value type.
     * \param revert - a functor that will be used to convert
     *                 the callee's value to the other's value type.
     * \return - a pair whose first element is a handle to a binding in the callee's list,
     *           and the second one is the other's binding handle
     */
    template<typename O, typename OT, typename TO>
    std::pair<BindingHandle, BindingHandle> bindTwoWay(Observable<O>& other, OT convert, TO revert)
    {
        std::shared_ptr<bool> lock(new bool(false));

        Binding b1 = other.addCallback(
                    [this, convert, lock](O value)
                    {
                        if (*lock)
                            return;
                        else
                            *lock = true;

                        this->set(convert(value));

                        *lock = false;
                    });

        BindingHandle h1 = addBinding(b1);

        Observable<O> *pOther = &other;
        CallbackPtr observeThis = addCallback(
                    [pOther, revert, lock](T value)
                    {
                        if (*lock)
                            return;
                        else
                            *lock = true;

                        pOther->set(revert(value));

                        *lock = false;
                    });

        BindingHandle h2 = other.addBinding(observeThis);
        return std::make_pair(h1, h2);
    }

    /*!< Overloaded binding method for the observable of the same type */
    std::pair<BindingHandle, BindingHandle> bindTwoWay(Observable<T>& other);
protected:
    /*!< A bare value setter, that is, the one that does not notify the observers */
    virtual void doSet(T& value) = 0;

    /*!< This method notifies observers when a new value was set by invoking the callbacks */
    void onChange(T newValue);
private:
    using StoredCallbackPtr = std::weak_ptr<Callback>;
    using Callbacks = std::deque<StoredCallbackPtr>;

    Callbacks _callbacks;
    bool _fireOnAdd;
    std::unordered_set<Binding> _bindings;
};

template<typename T>
void Observable<T>::set(T value)
{
    T oldValue = get();
    if (value != oldValue)
    {
        doSet(value);
        onChange(get());
    }
}

template<typename T>
void Observable<T>::onChange(T newValue)
{
    Callbacks newCallbacks;
    Callbacks *pNewCallbacks = &newCallbacks;
    std::for_each(_callbacks.begin(), _callbacks.end(),
    [newValue, pNewCallbacks] (StoredCallbackPtr& element) {
        // Note that we are filtering dead callbacks out
        if (CallbackPtr pCallback = element.lock())
        {
            pCallback->operator()(newValue);
            pNewCallbacks->emplace_back(pCallback);
        }
    });

    _callbacks = newCallbacks;
}

template<typename T>
void Observable<T>::removeCallback(CallbackPtr callback)
{
    auto pos = std::find(_callbacks.begin(), _callbacks.end(),
    [callback] (StoredCallbackPtr & e) {
        CallbackPtr stored = e.lock();
        if (!stored)
            return false;

        return (stored == callback);
    });

    if (pos != _callbacks.end())
        _callbacks.erase(pos);
}

template<typename T>
std::pair<typename Observable<T>::BindingHandle,
            typename Observable<T>::BindingHandle>
Observable<T>::bindTwoWay(Observable<T>& other)
{
    std::shared_ptr<bool> lock(new bool(false));

    Binding b1 = other.addCallback(
                [this, lock](T value)
                {
                    if (*lock)
                        return;
                    else
                        *lock = true;

                    this->set(value);

                    *lock = false;
                });

    BindingHandle h1 = addBinding(b1);

    Observable<T> *pOther = &other;
    CallbackPtr observeThis = addCallback(
                [pOther, lock](T value)
                {
                    if (*lock)
                        return;
                    else
                        *lock = true;

                    pOther->set(value);

                    *lock = false;
                });

    BindingHandle h2 = other.addBinding(observeThis);
    return std::make_pair(h1, h2);
}

#endif // OBSERVABLE_H
