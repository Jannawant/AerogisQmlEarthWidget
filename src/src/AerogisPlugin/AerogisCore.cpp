#include "AerogisCore.h"
#include <iostream>
#include <fstream>
#include <qdatetime.h>
#include <qstring.h>
#include <qtranslator.h>
#include <qcoreapplication.h>

AerogisCore::AerogisCore(QQuickItem *parent)
   : QQuickItem(parent)
{
}


void AerogisCore::log(const QString &s)
{
	try {
      std::ofstream logFile(mLogFileName.toStdString().c_str(), std::ofstream::app);
      logFile << (QDate::currentDate().toString("yyyy-MMM-dd ") + QTime::currentTime().toString("hh-mm-ss") + " : " + s + "\n").toStdString().c_str();
      logFile.close();
	}
	catch (...) {
		std::cerr << "error: Unhandled exception in AerogisCore::log\n";
	}

}

void AerogisCore::setGuiLanguage(const QString &_la)
{
   try {
      auto la = _la.toLower();
      if (translators.count(la) == 0)
      {
         // first use of this language - create QTranslator
         translators[la] = new QTranslator(this);
      }

      if (translators[la]->load("aerogis_" + la + ".qm", "./Aerogis/"))
      {
         // loaded ok
         qApp->installTranslator(translators[la]);
      }
      else
      {
         // load error
         std::cerr << "error: cannot load translation file for language '" << la.toStdString() << "'\n";
      }
   }
   catch (...) {
      std::cerr << "error: Unhandled exception in AerogisCore::switchLanguage\n";
   }

}
