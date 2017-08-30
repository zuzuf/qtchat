#include "textedit.h"
#include <QFile>
#include <QMimeData>
#include <QFileInfo>
#include <QImageReader>

#include <QDebug>

TextEdit::TextEdit(QWidget *parent) : QTextEdit(parent)
{

}

bool TextEdit::canInsertFromMimeData(const QMimeData* source) const
{
    return source->hasImage() || source->hasUrls() || QTextEdit::canInsertFromMimeData(source);
}

void TextEdit::insertFromMimeData(const QMimeData* source)
{
    if (source->hasImage())
    {
        static int i = 1;
        QUrl url(QString("dropped_image_%1").arg(i++));
        dropImage(url.toLocalFile());
    }
    else if (source->hasUrls())
    {
        foreach (QUrl url, source->urls())
        {
            QFileInfo info(url.toLocalFile());
            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
                dropImage(info.filePath());
            else
                dropTextFile(url);
        }
    }
    else
    {
        QTextEdit::insertFromMimeData(source);
    }
}

void TextEdit::dropImage(const QString &filepath)
{
    if (!filepath.isNull())
    {
        QFile file(filepath);
        if (file.open(QIODevice::ReadOnly))
        {
            QFileInfo info(filepath);
            const QByteArray data = file.readAll().toBase64();
            textCursor().insertHtml("<img src=\"data:image/" + info.suffix().toLower() + ";base64," + data + "\"/>");
        }
    }
}

void TextEdit::dropTextFile(const QUrl& url)
{
    QFile file(url.toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        textCursor().insertText(file.readAll());
}
