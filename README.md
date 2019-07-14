# simple-observable-qt
An example of using MVVM and Observer pattern with Qt.
I felt that Qt signals are still too low-level and do not provide the desired "observable property" notion, and on the other hand, the whole reactive programming machinery is an overkill, so a (relatively) simple Observable template class was implemented. While being simple, it supports bidirectional data bindings.

Reusable part of the code is in the lib/ folder.

To see how it is used, look at the interplay between MainView and MainViewModel classes (mainview.h, mainview.cpp, mainviewmodel.h, mainviewmodel.cpp).

Further reading:

- [Model-view-viewmodel](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93viewmodel)

- [Observer](https://en.wikipedia.org/wiki/Observer_pattern)
