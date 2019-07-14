#include "mainview.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <memory>

MainView::MainView() :
    _captionBinding(nullptr),
    _inputBinding1(nullptr),
    _delegate(nullptr)
{
    _toolbar = new QToolBar();
    addToolBar(_toolbar);

    _captionLabel = new QLabel();
    _toolbar->addWidget(_captionLabel);

    QWidget *central = new QWidget();
    setCentralWidget(central);

    QVBoxLayout *layout = new QVBoxLayout();
    central->setLayout(layout);

    _input1 = new QLineEdit(central);
    layout->addWidget(_input1);
    _inputBinding1 = std::make_shared<InputBinding>(
            [this]() { return _input1->text(); },
            [this](QString text) { _input1->setText(text); },
            _input1,
            &QLineEdit::textChanged
    );

    _input2 = new QLineEdit(central);
    layout->addWidget(_input2);
    _inputBinding2 = std::make_shared<InputBinding>(
            [this]() { return _input2->text(); },
            [this](QString text) { _input2->setText(text); },
            _input2,
            &QLineEdit::textChanged
    );

    _moreBtn = new QPushButton(central);
    layout->addWidget(_moreBtn);
    _moreConnection = QObject::connect(_moreBtn, &QPushButton::clicked,
    [this](bool checked)
    {
        if (MainViewModelPtr vm = viewModel())
            _delegate->showMain(vm->clone());
    });
}

MainView::~MainView()
{
    QObject::disconnect(_moreConnection);
}

void MainView::bind()
{
    MainViewModelPtr vm = viewModel();
    if (!vm)
        return;

    _captionBinding = vm->caption().addCallback(
                [this] (std::string newV) {
                    QString newText = QString::fromStdString(newV);
                    this->_captionLabel->setText(newText);
                });

    _moreTitleBinding = vm->more().addCallback(
                [this] (std::string newV) {
                    QString newText = QString::fromStdString(newV);
                    this->_moreBtn->setText(newText);
                });

    auto strToQ = [](const std::string& str)
    {
        return QString::fromStdString(str);
    };

    auto qToStr = [](const QString& qstr)
    {
        return qstr.toStdString();
    };

    _inputBinding1->bindTwoWay(vm->text(), strToQ, qToStr);
    _inputBinding2->bindTwoWay(vm->text(), strToQ, qToStr);
    sizeHandle().bindTwoWay(vm->size());
}

void MainView::unbind()
{
    _captionBinding = nullptr;
    _moreTitleBinding = nullptr;

    MainViewModelPtr vm = viewModel();
    if (vm)
        vm->text().unbind();

    _inputBinding1->unbind();
    _inputBinding2->unbind();
}

MainView::Delegate *MainView::delegate() const
{
    return _delegate;
}

void MainView::setDelegate(Delegate *delegate)
{
    _delegate = delegate;
}

MainView::Delegate::~Delegate()
{

}
