#include "widget.h"
#include "ui_widget.h"
#include "ButtonItemDelegate.h"
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ptr_model_ = new QStandardItemModel(this);
    ptr_filter_model_ = new QSortFilterProxyModel(this);
    ptr_filter_model_->setFilterRole(Qt::StatusRole);
    ptr_filter_model_->setFilterKeyColumn(2);
    ptr_filter_model_->setSourceModel(ptr_model_);

    ui->tableView->setModel(ptr_filter_model_);
    ui->tableView->setMouseTracking(true);

    CButtonItemDelegate* ptr_item_delegate = new CButtonItemDelegate(this);
    connect(ptr_item_delegate, &CButtonItemDelegate::viewClicked, this, &Widget::onViewClicked);
    connect(ptr_item_delegate, &CButtonItemDelegate::closeClicked, this, &Widget::onCloseClicked);
    ui->tableView->setItemDelegateForColumn(3, ptr_item_delegate);

    connect(ui->open_dir_button, &QAbstractButton::clicked, this, &Widget::onOpenDir);
    connect(ui->dir_button, &QAbstractButton::clicked, this, &Widget::onDir);
    connect(ui->file_button, &QAbstractButton::clicked, this, &Widget::onFile);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onOpenDir()
{
    QString str_dirname = QFileDialog::getExistingDirectory();
    if (str_dirname.isEmpty())
    {
        return;
    }
    ui->lineEdit->setText(str_dirname);

    ptr_model_->clear();
    ptr_model_->setHorizontalHeaderLabels(QStringList() << "Name" << "Size" << "Status" << "Operator");

    int n_row = 0;
    QDir dir(str_dirname);
    for (auto file_info: dir.entryInfoList(QDir::Dirs | QDir::Files))
    {
        if (file_info.fileName() == "." || file_info.fileName() == "..")
        {
            continue;
        }

        ui->tableView->setRowHeight(n_row, 40);
        ptr_model_->setItem(n_row, 0, new QStandardItem(file_info.fileName()));
        ptr_model_->setItem(n_row, 1, new QStandardItem(QString::number(file_info.size())));

        bool b_file = file_info.isFile();
        QStandardItem* item = new QStandardItem(b_file ? "File" : "Dir");
        item->setData(b_file, Qt::StatusRole);
        ptr_model_->setItem(n_row, 2, item);
        ++n_row;
    }
}

void Widget::onFile()
{
    ptr_filter_model_->setFilterRegExp("true");
}

void Widget::onDir()
{
    ptr_filter_model_->setFilterRegExp("false");
}

void Widget::onCloseClicked(const QModelIndex &index)
{
    ptr_model_->removeRow(index.row());
}

void Widget::onViewClicked(const QModelIndex &index)
{
    QString str_file_name = ptr_model_->item(index.row())->text();
    QString str_dir_name = ui->lineEdit->text();
    QDesktopServices::openUrl(QUrl::fromLocalFile(str_dir_name + "/" + str_file_name));
}
