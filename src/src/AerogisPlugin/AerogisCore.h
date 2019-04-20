#ifndef AEROGISCORE_H
#define AEROGISCORE_H

#include <QQuickItem>
#include <QString>
#include <qtranslator.h>

/** QML item to process Aerogis core events */
class AerogisCore : public QQuickItem
{
   Q_OBJECT;
   Q_PROPERTY(QString logFileName MEMBER mLogFileName);

public:
   explicit AerogisCore(QQuickItem *parent = 0);

signals:

public slots:
	/** main AerogisWidget's log */
	void log(const QString &s);
   void setGuiLanguage(const QString &la);

private:
   QString mLogFileName;
   std::map<QString, QTranslator*> translators;
};

#endif // AEROGISCORE_H
