#include "textedit.h"
#include <QFile>
#include <QMimeData>
#include <QFileInfo>
#include <QImageReader>
#include <QBuffer>

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

void TextEdit::dropImage(const QImage &image)
{
    if (!image.isNull())
    {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "jpeg", 75);
        const QByteArray data = buffer.buffer().toBase64();
        textCursor().insertHtml("<img src=\"data:image/jpg;base64," + data + "\"/>");
    }
}

void TextEdit::dropTextFile(const QUrl& url)
{
    QFile file(url.toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        textCursor().insertText(file.readAll());
}
