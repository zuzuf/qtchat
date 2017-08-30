#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>

class TextEdit : public QTextEdit
{
public:
    TextEdit(QWidget *parent = nullptr);

protected:
    virtual bool canInsertFromMimeData(const QMimeData* source) const;

    virtual void insertFromMimeData(const QMimeData* source);

private:
    void dropImage(const QString& filepath);

    void dropTextFile(const QUrl& url);
};
#endif // TEXTEDIT_H
