#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "lib/view.h"
#include "mainviewmodel.h"
#include <QStackedLayout>
#include <QLabel>
#include <QToolBar>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include "lib/uibinding.h"

class MainView : public View<MainViewModel, QMainWindow>
{
public:
    MainView();
    virtual ~MainView();

    class Delegate
    {
    public:
        virtual void showMain(MainViewModelPtr model) = 0;
        virtual ~Delegate();
    };

    Delegate *delegate() const;
    void setDelegate(Delegate *delegate);

protected:
    void bind();
    void unbind();
private:
    using Text = MainViewModel::Text;
    using TextBinding = Text::CallbackPtr;
    using InputBinding = UIBinding<QString>;
    using IBPtr = std::shared_ptr<InputBinding>;

    QLabel *_captionLabel;
    TextBinding _captionBinding;

    QToolBar *_toolbar;

    QLineEdit *_input1;
    IBPtr _inputBinding1;

    QLineEdit *_input2;
    IBPtr _inputBinding2;

    QPushButton *_moreBtn;
    TextBinding _moreTitleBinding;
    QMetaObject::Connection _moreConnection;

    Delegate *_delegate;
};

#endif // MAINVIEW_H
