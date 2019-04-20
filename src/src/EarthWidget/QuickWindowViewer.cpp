#include "QuickWindowViewer.hpp"

#include <QOpenGLFramebufferObject>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>
#include <QQuickItem>

#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <osgViewer/CompositeViewer>
#include <osg/State>
#include <osg/StateSet>

#include <map>
#include <QDateTime>
#include <iostream>
#include <algorithm>

#include "ShaderFix.h"

namespace osgQtQuick {

   void AerogisViewer::frame(double simulationTime)
   {
      if (_done) return;

      frameUpdates(simulationTime);

      auto begT = QTime::currentTime();

      renderingTraversals();

      int frT = begT.msecsTo(QTime::currentTime());

      // frame time (FPS) counter
      static double S = 0, S2 = 0;
      S += frT;
      //S2 += frT*frT;
      static int mi = 10000, ma = 0;
      mi = std::min(frT, mi);
      ma = std::max(frT, ma);
      static int prf = 0;
      prf++;
      prf %= 100;
      if (prf == 0)
      {
         double M = S * 0.01;    // average
         //double D = S2 - M*S;  // sum of square diffs
         //double Err = sqrt(D / 99);   // root mean square 
         S = S2 = 0;
         //std::cout << "frame time " << (int)M << " +- " << (int)Err << " ms\n";
         if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
         {
            std::cout << "frame time " << mi << "-" << ma << " avg " << M << " ms\n";
         }
         mi = 10000; ma = 0;
      }

   }


struct QuickWindowViewer::Hidden: public QObject
{
    Q_OBJECT

public:
    Hidden(QuickWindowViewer *viewer, QQuickWindow *window) :
        QObject(viewer),
        viewer(0),
        window(0),
        frameTimer(-1)
    {
        acceptViewer(viewer);
        acceptWindow(window);
        initCompositeViewer();
    }

    ~Hidden() {
    }

    QuickWindowViewer *viewer;
    QQuickWindow *window;
    osg::ref_ptr<AerogisViewer> compositeViewer;
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow;

    int frameTimer;

    typedef std::map<QQuickWindow*, QuickWindowViewer*> ViewersMap;
    static ViewersMap viewers;

protected:
    void timerEvent(QTimerEvent *event)
    {
       try
       {
          if (event->timerId() == frameTimer) {
             if (window)
                window->update();
          }

          QObject::timerEvent(event);
       }
       catch (...)
       {
          std::cerr << "Draw Error: unhandled exception in QuickWindowViewer::Hidden::timerEvent(...) !\n";
       }
    }


private slots:
   void frame()
   {
      if (!compositeViewer.valid()) return;

      // Qt bug!?
      auto *glContext = QOpenGLContext::currentContext();
      if( glContext )
      {
         auto *functions = glContext->functions();

         if (!ShaderFix::isSetGL2Support)
            ShaderFix::setGL2Support();

#if QT_VERSION >= 0x50300
			GLint old_program;
			functions->glGetIntegerv(GL_CURRENT_PROGRAM, &old_program);
			functions->glUseProgram(0);

         if ( viewer )
            emit viewer->viewerDrawsFrame(compositeViewer.get());

         int rl;
         functions->glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rl);
         if (rl > 0)
         {
            if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
            {
               std::cerr << "GL_UNPACK_ROW_LENGTH = " << rl << " after draw frame. Setting to 0.\n";
            }
            functions->glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
         }
#else
			functions->glUseProgram(0);
			compositeViewer->frame();
#endif
      }
   }

private:
    void acceptViewer(QuickWindowViewer *viewer) {
        if (this->viewer == viewer) return;
        this->viewer = viewer;
        if (parent() != viewer) setParent(viewer);
    }

    void acceptWindow(QQuickWindow *window) {
        if (this->window == window) return;
        if (this->window) disconnect(this->window);
        this->window = window;
        if (viewer->parent() != window) viewer->setParent(window);
        connect(window, SIGNAL(beforeRendering()),
           this, SLOT(frame()), Qt::DirectConnection);
        // Запускаем постоянное обновление
        if (frameTimer >= 0) killTimer(frameTimer);
        if (window) {
            window->setClearBeforeRendering(false);
            frameTimer = startTimer(10);
        }
    }

    void initCompositeViewer() {
        compositeViewer = new AerogisViewer();
        compositeViewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
        // disable the default setting of viewer.done() by pressing Escape.
        compositeViewer->setKeyEventSetsDone(0);
        graphicsWindow = new osgViewer::GraphicsWindowEmbedded(0, 0,
                                                               window->width(),
                                                               window->height());
    }
};

QuickWindowViewer::Hidden::ViewersMap QuickWindowViewer::Hidden::viewers;

QuickWindowViewer::QuickWindowViewer(QQuickWindow *window) :
    QObject(window),
    h(new Hidden(this, window))
{
	// set as callback a lambda that is signaling eventsProcessed() signal of *this* QuickWindowViewer object
	h->compositeViewer.get()->setEventsProcessedFunc( [this](){ emit eventsProcessed(); } );
}

QuickWindowViewer::~QuickWindowViewer()
{
    Hidden::viewers.erase(h->window);
}

osg::GraphicsContext *QuickWindowViewer::graphicsContext()
{
    return h->graphicsWindow.get();
}

osgViewer::CompositeViewer *QuickWindowViewer::compositeViewer()
{
    return h->compositeViewer.get();
}

QuickWindowViewer *QuickWindowViewer::instance(QQuickWindow *window)
{
    if (!window) return 0;
    QuickWindowViewer *viewer = 0;
    Hidden::ViewersMap::iterator it = Hidden::viewers.find(window);
    if (it != Hidden::viewers.end()) {
        viewer = it->second;
    } else {
        viewer = new QuickWindowViewer(window);
        Hidden::viewers.insert(std::make_pair(window, viewer));
    }
    return viewer;
}

} // namespace osgQtQuick

#include "QuickWindowViewer.moc"
