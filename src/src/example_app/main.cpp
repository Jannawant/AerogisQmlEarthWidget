#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef WIN32
   // test if there are dll-s
#ifdef NDEBUG
   if (::LoadLibrary(L"osg130-osg.dll") == 0)
      std::cerr << "osg130-osg.dll not found\n";
   if (::LoadLibrary(L"osgEarth.dll") == 0)
      std::cerr << "osgEarth.dll not found\n";
#endif
#endif

   try
   {

      QGuiApplication app(argc, argv);

      QQmlApplicationEngine engine;
      engine.load("main.qml");

      return app.exec();

   }
   catch (std::exception &e)
   {
      std::cerr << "Exception [" << e.what() << "] in example_app. Terminated.\n";
   }
   catch (...)
   {
      std::cerr << "Unknown exception in example_app. Terminated.\n";
   }
   return 1;
}
