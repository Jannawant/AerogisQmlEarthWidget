#ifndef OSGQTQUICK_QUICKWINDOWVIEWER_HPP
#define OSGQTQUICK_QUICKWINDOWVIEWER_HPP

#include "Export.hpp"
#include <QObject>
#include <osgViewer/CompositeViewer>

QT_BEGIN_NAMESPACE
class QQuickWindow;
QT_END_NAMESPACE

namespace osg {
class GraphicsContext;
} // namespace osg

namespace osgViewer {
class CompositeViewer;
}

namespace osgQtQuick {

      class AerogisViewer : public osgViewer::CompositeViewer
      {
         std::function<void()> _eventsProcessedFunc;
      public:
         void setEventsProcessedFunc(std::function<void()> udf)
         {
            _eventsProcessedFunc = udf;
         }

         virtual void frame(double simulationTime = USE_REFERENCE_TIME);	// main draw routine (calls frameUpdates() and renderingTraversals() )

         virtual void frameUpdates(double simulationTime = USE_REFERENCE_TIME)
         {
            if (_firstFrame)
            {
               viewerInit();

               if (!isRealized())
               {
                  realize();
               }

               _firstFrame = false;
            }
            advance(simulationTime);

            eventTraversal();

            if (_eventsProcessedFunc)
               _eventsProcessedFunc();	// signal that we've done update()

            updateTraversal();
         }
      };




class OSGQTQUICK_EXPORT QuickWindowViewer : public QObject
{
    Q_OBJECT

public:
    QuickWindowViewer(QQuickWindow *window);
    ~QuickWindowViewer();

    osg::GraphicsContext* graphicsContext();
    osgViewer::CompositeViewer* compositeViewer();

    static QuickWindowViewer *instance(QQuickWindow *window);

 signals:
	 void eventsProcessed();
    void viewerDrawsFrame(AerogisViewer *);

private:
    struct Hidden;
    friend struct Hidden;
    Hidden *h;
};

} // namespace osgQtQuick

#endif // OSGQTQUICK_QUICKWINDOWVIEWER_HPP
