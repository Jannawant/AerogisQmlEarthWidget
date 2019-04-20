#ifndef _H_OSGQTQUICK_OSGVIEPORT_H_
#define _H_OSGQTQUICK_OSGVIEPORT_H_

#include "Export.hpp"

#include <QQuickItem>
#include <qcolor.h>
#include <osgViewer/Viewer>
#include <QOpenGLFramebufferObject>
#include "QuickWindowViewer.hpp"

namespace osgQtQuick {

class OSGNode;

class OSGQTQUICK_EXPORT OSGViewport : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
	 Q_PROPERTY(DrawingMode mode READ mode WRITE setMode NOTIFY modeChanged)

    Q_ENUMS(DrawingMode)

public:
    enum DrawingMode {
        Native,
        Buffer
    };

    explicit OSGViewport(QQuickItem *parent = 0);
    
    void setDarawingMode(DrawingMode mode);

    QColor color() const;
    void setColor(const QColor &color);

    DrawingMode mode() const;
    void setMode(DrawingMode mode);

signals:
    void colorChanged(const QColor &color);
    void modeChanged(DrawingMode mode);
    void frameEventsProcessed();
    void frameDrawn(QOpenGLFramebufferObject *fb);
    void ownCameraEvent();

public slots:
    virtual void viewerDrawsFrame(AerogisViewer *v);		// this event is called inside render pipeline, and usually needs to do just v->frame()
    virtual void viewerDrawnFrame();
    void myplusCamera();
    void myminusCamera();

protected:

    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    QSGNode* updatePaintNode(QSGNode *oldNode,
                             UpdatePaintNodeData *updatePaintNodeData);

    osgViewer::View *view();

private:
    struct Hidden;
    friend struct Hidden;
    Hidden *h;
};

} // namespace osgQtQuick

#endif // _H_OSGQTQUICK_OSGVIEPORT_H_
