#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);

signals:

public slots:

private:
    static AboutDialog *s_instance;
public:
    static AboutDialog *instance();
};

#endif // ABOUTDIALOG_H
