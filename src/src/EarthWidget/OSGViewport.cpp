#include "OSGViewport.hpp"

#include "OSGNode.hpp"
#include "QuickWindowViewer.hpp"

#include <QQuickWindow>
#include <QOpenGLFramebufferObject>
#include <QSGSimpleTextureNode>
#include <QDebug>

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgEarthUtil/Controls>
#include <qdatetime.h>

#include "ShaderFix.h"
#include "QOpenGLContext"
#include "QuickWindowViewer.hpp"

namespace osgQtQuick {

// Copied from "GraphicsWindowQt.cpp" (osg module osgQt)
class QtKeyboardMap
{

public:
    QtKeyboardMap()
    {
        mKeyMap[Qt::Key_Escape     ] = osgGA::GUIEventAdapter::KEY_Escape;
        mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Home       ] = osgGA::GUIEventAdapter::KEY_Home;
        mKeyMap[Qt::Key_Enter      ] = osgGA::GUIEventAdapter::KEY_KP_Enter;
        mKeyMap[Qt::Key_End        ] = osgGA::GUIEventAdapter::KEY_End;
        mKeyMap[Qt::Key_Return     ] = osgGA::GUIEventAdapter::KEY_Return;
        mKeyMap[Qt::Key_PageUp     ] = osgGA::GUIEventAdapter::KEY_Page_Up;
        mKeyMap[Qt::Key_PageDown   ] = osgGA::GUIEventAdapter::KEY_Page_Down;
        mKeyMap[Qt::Key_Left       ] = osgGA::GUIEventAdapter::KEY_Left;
        mKeyMap[Qt::Key_Right      ] = osgGA::GUIEventAdapter::KEY_Right;
        mKeyMap[Qt::Key_Up         ] = osgGA::GUIEventAdapter::KEY_Up;
        mKeyMap[Qt::Key_Down       ] = osgGA::GUIEventAdapter::KEY_Down;
        mKeyMap[Qt::Key_Backspace  ] = osgGA::GUIEventAdapter::KEY_BackSpace;
        mKeyMap[Qt::Key_Tab        ] = osgGA::GUIEventAdapter::KEY_Tab;
        mKeyMap[Qt::Key_Space      ] = osgGA::GUIEventAdapter::KEY_Space;
        mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Alt        ] = osgGA::GUIEventAdapter::KEY_Alt_L;
        mKeyMap[Qt::Key_Shift      ] = osgGA::GUIEventAdapter::KEY_Shift_L;
        mKeyMap[Qt::Key_Control    ] = osgGA::GUIEventAdapter::KEY_Control_L;
        mKeyMap[Qt::Key_Meta       ] = osgGA::GUIEventAdapter::KEY_Meta_L;

        mKeyMap[Qt::Key_F1         ] = osgGA::GUIEventAdapter::KEY_F1;
        mKeyMap[Qt::Key_F2         ] = osgGA::GUIEventAdapter::KEY_F2;
        mKeyMap[Qt::Key_F3         ] = osgGA::GUIEventAdapter::KEY_F3;
        mKeyMap[Qt::Key_F4         ] = osgGA::GUIEventAdapter::KEY_F4;
        mKeyMap[Qt::Key_F5         ] = osgGA::GUIEventAdapter::KEY_F5;
        mKeyMap[Qt::Key_F6         ] = osgGA::GUIEventAdapter::KEY_F6;
        mKeyMap[Qt::Key_F7         ] = osgGA::GUIEventAdapter::KEY_F7;
        mKeyMap[Qt::Key_F8         ] = osgGA::GUIEventAdapter::KEY_F8;
        mKeyMap[Qt::Key_F9         ] = osgGA::GUIEventAdapter::KEY_F9;
        mKeyMap[Qt::Key_F10        ] = osgGA::GUIEventAdapter::KEY_F10;
        mKeyMap[Qt::Key_F11        ] = osgGA::GUIEventAdapter::KEY_F11;
        mKeyMap[Qt::Key_F12        ] = osgGA::GUIEventAdapter::KEY_F12;
        mKeyMap[Qt::Key_F13        ] = osgGA::GUIEventAdapter::KEY_F13;
        mKeyMap[Qt::Key_F14        ] = osgGA::GUIEventAdapter::KEY_F14;
        mKeyMap[Qt::Key_F15        ] = osgGA::GUIEventAdapter::KEY_F15;
        mKeyMap[Qt::Key_F16        ] = osgGA::GUIEventAdapter::KEY_F16;
        mKeyMap[Qt::Key_F17        ] = osgGA::GUIEventAdapter::KEY_F17;
        mKeyMap[Qt::Key_F18        ] = osgGA::GUIEventAdapter::KEY_F18;
        mKeyMap[Qt::Key_F19        ] = osgGA::GUIEventAdapter::KEY_F19;
        mKeyMap[Qt::Key_F20        ] = osgGA::GUIEventAdapter::KEY_F20;

        mKeyMap[Qt::Key_hyphen     ] = '-';
        mKeyMap[Qt::Key_Equal      ] = '=';

        mKeyMap[Qt::Key_division   ] = osgGA::GUIEventAdapter::KEY_KP_Divide;
        mKeyMap[Qt::Key_multiply   ] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
        mKeyMap[Qt::Key_Minus      ] = '-';
        mKeyMap[Qt::Key_Plus       ] = '+';
        //mKeyMap[Qt::Key_H          ] = osgGA::GUIEventAdapter::KEY_KP_Home;
        //mKeyMap[Qt::Key_           ] = osgGA::GUIEventAdapter::KEY_KP_Up;
        //mKeyMap[92                 ] = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
        //mKeyMap[86                 ] = osgGA::GUIEventAdapter::KEY_KP_Left;
        //mKeyMap[87                 ] = osgGA::GUIEventAdapter::KEY_KP_Begin;
        //mKeyMap[88                 ] = osgGA::GUIEventAdapter::KEY_KP_Right;
        //mKeyMap[83                 ] = osgGA::GUIEventAdapter::KEY_KP_End;
        //mKeyMap[84                 ] = osgGA::GUIEventAdapter::KEY_KP_Down;
        //mKeyMap[85                 ] = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
        mKeyMap[Qt::Key_Insert     ] = osgGA::GUIEventAdapter::KEY_KP_Insert;
        //mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_KP_Delete;
    }

    ~QtKeyboardMap()
    {
    }

    int remapKey(QKeyEvent* event)
    {
        KeyMap::iterator itr = mKeyMap.find(event->key());
        if (itr == mKeyMap.end())
        {
            return int(*(event->text().toLatin1().data()));
        }
        else
            return itr->second;
    }

    private:
    typedef std::map<unsigned int, int> KeyMap;
    KeyMap mKeyMap;
};

struct OSGViewport::Hidden : public QObject
{
    Q_OBJECT

    struct PreDraw: public osg::Camera::DrawCallback {
    public:
        PreDraw(Hidden *h);

        void operator ()(osg::RenderInfo &renderInfo) const;

        mutable Hidden *h;
    };
    friend struct PreDraw;

    struct PostDraw: public osg::Camera::DrawCallback {
    public:
        PostDraw(Hidden *h);

        void operator ()(osg::RenderInfo &renderInfo) const;

        mutable Hidden *h;
    };
    friend struct PostDraw;

public:

    // Конструктор
    Hidden(OSGViewport *quickItem) :
        QObject(quickItem),
        quickItem(quickItem),
        drawingMode(OSGViewport::Buffer),
        mode(OSGViewport::Native),
        fbo(0),
        texture(0),
        textureNode(0)
    {
        //qDebug()<<"CONSTRUCT HIDDEN";
        // Создание сцены и вьювера
        initOSG();
        //qDebug()<<"CONSTRUCT HIDDEN inited osg";
        Q_ASSERT(quickItem);
        connect(quickItem, SIGNAL(windowChanged(QQuickWindow*)),
			  this, SLOT(onWindowChanged(QQuickWindow*)));

    }

    // Деструктор
    ~Hidden() {
        //my changes
        //delete view;
        //delete quickItem;
        //delete texture;
        //delete textureNode;
        //delete fbo;
    }

    QPointF mousePoint(QMouseEvent *event) {
       qreal x = (event->x() - quickItem->width()) / quickItem->width();
       qreal y = (event->y() - quickItem->height()) / quickItem->height();
       return QPointF(x, y);
    }

    bool acceptMode(OSGViewport::DrawingMode mode) {
        if (this->mode == mode) return false;

        this->mode = mode;
        if (mode == OSGViewport::Buffer) {
            if(!preDraw.valid()) {
                //qDebug()<<Q_FUNC_INFO << "TRY create PREDRAW";
                preDraw = new PreDraw(this);
                //qDebug()<<Q_FUNC_INFO << "Created PREDRAW. try set draw callback";
                view->getCamera()->setPreDrawCallback(preDraw.get());
                //qDebug()<<Q_FUNC_INFO << "Set draw callback";
            }
            if(!postDraw.valid()) {
                //qDebug()<<Q_FUNC_INFO << "Create POSTDRAW";
                postDraw = new PostDraw(this);
                //qDebug()<<Q_FUNC_INFO << "Try set Post callback";
                view->getCamera()->setPostDrawCallback(postDraw.get());
                //qDebug()<<"Set Post callback";
            }
        }

        return true;
    }

    void setKeyboardModifiers(QInputEvent* event)
    {
        int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier);
        unsigned int mask = 0;
        if ( modkey & Qt::ShiftModifier ) mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
        if ( modkey & Qt::ControlModifier ) mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;
        if ( modkey & Qt::AltModifier ) mask |= osgGA::GUIEventAdapter::MODKEY_ALT;
        view->getEventQueue()->getCurrentEventState()->setModKeyMask( mask );
    }

    // Public Qt data
    OSGViewport *quickItem;

    // Public data
    OSGViewport::DrawingMode drawingMode;



    // Public OSG data
    osg::ref_ptr<osgViewer::View> view;

    OSGViewport::DrawingMode mode;

    QOpenGLFramebufferObject *fbo;
    QSGTexture *texture;
    QSGSimpleTextureNode *textureNode;

    osg::ref_ptr<PreDraw> preDraw;
    osg::ref_ptr<PostDraw> postDraw;

    static QtKeyboardMap keyMap;

public slots:
    void updateViewport() {
        if (!quickItem->window())
        {
            //qDebug()<<"UPDATE VIEWPORT NOT WINDOW! ";
            return;
        }
        if (mode == OSGViewport::Native) {
            //qDebug()<<"MODE = NATIVE";
            QRectF rect = quickItem->mapRectToItem(0, quickItem->boundingRect());
            view->getCamera()->setViewport(rect.x(),
                                           quickItem->window()->height() - ( rect.y() + rect.height()),
                                           rect.width(),
                                           rect.height());
            view->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(rect.width())/static_cast<double>(rect.height()), 1.0f, 10000.0f );
        }
        if (mode == OSGViewport::Buffer) {
            //qDebug()<<"MODE = BUFFER";
            QSize size(quickItem->boundingRect().size().toSize());
            view->getCamera()->setViewport(0, 0, size.width(), size.height());
            view->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(size.width())/static_cast<double>(size.height()), 1.0f, 10000.0f );
            if (texture && texture->textureSize() != size) {
                updateFBO();
            }
        }
    }


signals:
    void frameDrawn(QOpenGLFramebufferObject*);

private slots:


    // Обработка события смены окна для элемента
    void onWindowChanged(QQuickWindow *window) {
        //qDebug()<<"ON window changed";
        if (QuickWindowViewer *qwv = QuickWindowViewer::instance(window)) {
            //qDebug()<<"Set graphic context";
            view->getCamera()->setGraphicsContext(qwv->graphicsContext());
            //qDebug()<<Q_FUNC_INFO<<"Update viewport";
            updateViewport();
            //qDebug()<<"Add view";
            qwv->compositeViewer()->addView(view.get());
            connect(window, SIGNAL(widthChanged(int)),
                    this, SLOT(updateViewport()));
            connect(window, SIGNAL(heightChanged(int)),
                    this, SLOT(updateViewport()));
            connect(qwv, SIGNAL(eventsProcessed()),
					quickItem, SIGNAL(frameEventsProcessed()));
            connect(qwv, SIGNAL(viewerDrawsFrame(AerogisViewer*)),
               quickItem, SLOT(viewerDrawsFrame(AerogisViewer*)), Qt::DirectConnection);
            connect(window, SIGNAL(afterRendering()),
               quickItem, SLOT(viewerDrawnFrame()), Qt::DirectConnection);
            //qDebug()<<"Complete on window changed";
        }
    }

    void onNodeChanged(osg::Node *node) {
        //qDebug()<<"On node changed";
        if (view.valid()) {
            //qDebug()<<"View is valid";
            view->setSceneData(node);
            view->home();
        }
        //qDebug()<<"Complete onNodeChanged";
    }

private:
    void initOSG() {
        view = new osgViewer::View();
        //qDebug()<<"INIT OSG";
    }

    void initFBO() {
        //qDebug()<<"try init FBO";
        //if(!quickItem) qDebug()<< "QuickItem not created";
        //else qDebug()<<"Adress quickitem"<< quickItem;
        QRectF rect = quickItem->mapRectToItem(0, quickItem->boundingRect());
        //qDebug()<<"try create  QOpenGLFramebufferObjectFormat";
        QOpenGLFramebufferObjectFormat format;
        //qDebug()<<"try set  Attachment";
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        //qDebug()<<"try set  size";
        QSize size(rect.size().toSize());
        //qDebug()<<"try create fbo";
        fbo = new QOpenGLFramebufferObject(size, format);
        //qDebug()<<"try create texture";
        texture = quickItem->window()->createTextureFromId(fbo->texture(), size);
        //qDebug()<<"try create QSGSimpleTextureNode";
        textureNode = new QSGSimpleTextureNode();
        textureNode->setRect(0, quickItem->height(), quickItem->width(), -quickItem->height());
        textureNode->setTexture(texture);
        quickItem->setFlag(QQuickItem::ItemHasContents, true);
        //qDebug()<<"try update viewport";
        updateViewport();
        //qDebug()<<"try update quickItem";
        quickItem->update();
        //qDebug()<<"complete initFBO";
    }

    void updateFBO()
    {
        //qDebug()<<Q_FUNC_INFO<<"Try update FBO";
        if (fbo) delete fbo;
		fbo = nullptr;
        //qDebug()<<Q_FUNC_INFO << fbo;
		QOpenGLContext *ctx = QOpenGLContext::currentContext();
        //qDebug() << "Current context" << ctx;
		if (!ctx)
			return;

        QRectF rect = quickItem->mapRectToItem(0, quickItem->boundingRect());
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        QSize size(rect.size().toSize());
        fbo = new QOpenGLFramebufferObject(size, format);
        //qDebug()<<Q_FUNC_INFO<<"Created FBO"<< fbo;
        if (texture) delete texture;
        texture = quickItem->window()->createTextureFromId(fbo->texture(), size);
        textureNode = new QSGSimpleTextureNode();
        textureNode->setRect(0, quickItem->height(), quickItem->width(), -quickItem->height());
        textureNode->setTexture(texture);
        quickItem->update();
    }

};

void OSGViewport::myplusCamera(){
    h->view->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN);
    //emit ownCameraEvent();
}

void OSGViewport ::myminusCamera(){
     h->view->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);
     //emit ownCameraEvent();
}


QtKeyboardMap OSGViewport::Hidden::keyMap = QtKeyboardMap();

/* ----------------------------------------------- struct Hidden::PreDraw --- */

OSGViewport::Hidden::PreDraw::PreDraw(Hidden *h) : h(h) {
}

void OSGViewport::Hidden::PreDraw::operator ()(osg::RenderInfo &renderInfo) const
{
    auto *st = reinterpret_cast<osg::StateSh*>(renderInfo.getState());
    auto &ls = st->getVertexAttribArrayList();
    for (auto &i : ls)
    {
       i._enabled = false;
    }

    if (!h->fbo)
    {
        //qDebug()<<Q_FUNC_INFO <<" if !h->fbo";
        h->initFBO();
    }
	if (h->fbo)
	{
		QOpenGLContext *ctx = QOpenGLContext::currentContext();
        //qDebug() << "ctx = " << ctx << "h->fbo->isValid"<< h->fbo->isValid();
        //qDebug() << "h->fbo->isValid"<<h->fbo->isValid();
		if (ctx && h->fbo->isValid())
		{
            //qDebug()<<"TRY BIND";
			h->fbo->bind();
            //qDebug()<<"H-FBO binded";
		}
		else
		{
			std::cerr << "fbo error\n";
		}
	}
}

/* ----------------------------------------------- struct Hidden::PostDraw --- */

OSGViewport::Hidden::PostDraw::PostDraw(Hidden *h) : h(h) {
}

void OSGViewport::Hidden::PostDraw::operator ()(osg::RenderInfo &/*renderInfo*/) const
{
    //qDebug()<<"POST_DRAW operator() frameDrawn emited";
    emit h->frameDrawn(h->fbo);
    //qDebug()<<"POST_DRAW operator() try binDefault";
    if (h->fbo) h->fbo->bindDefault();
    //qDebug()<<"POST DRAW bind to Default";
}

/* ---------------------------------------------------- class OSGViewport --- */

OSGViewport::OSGViewport(QQuickItem *parent) :
    QQuickItem(parent), h(new Hidden(this))
{
   //qDebug()<<"OSGVIEWPORT CONSTRUCT" << "QQUickItem parent = " << parent;
   connect(h, SIGNAL(frameDrawn(QOpenGLFramebufferObject*)), this, SIGNAL(frameDrawn(QOpenGLFramebufferObject*)), Qt::ConnectionType::DirectConnection);
   setAcceptHoverEvents(true);
   setAcceptedMouseButtons(Qt::AllButtons);
}

void OSGViewport::setDarawingMode(OSGViewport::DrawingMode mode)
{
    if (h->drawingMode != mode) {
        h->drawingMode = mode;
    }
    //qDebug()<<Q_FUNC_INFO<< "Setted drawing mode";
}

QColor OSGViewport::color() const
{
    const osg::Vec4 osgColor = h->view->getCamera()->getClearColor();
    return QColor::fromRgbF(
                osgColor.r(),
                osgColor.g(),
                osgColor.b(),
                osgColor.a());
}

void OSGViewport::setColor(const QColor &color)
{
    osg::Vec4 osgColor(
                color.redF(),
                color.greenF(),
                color.blueF(),
                color.alphaF());
    if (h->view->getCamera()->getClearColor() != osgColor) {
        h->view->getCamera()->setClearColor(osgColor);
        emit colorChanged(color);
    }
}

OSGViewport::DrawingMode OSGViewport::mode() const
{
    return h->mode;
}

void OSGViewport::setMode(OSGViewport::DrawingMode mode)
{
    if (h->acceptMode(mode)) emit modeChanged(mode);
}

void OSGViewport::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (window()) h->updateViewport();

    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

void OSGViewport::mouseDoubleClickEvent(QMouseEvent *event)
{
    //qDebug() << "mouse dclick:" << event;
    int button = 0;
    switch ( event->button() )
    {
        case Qt::LeftButton: button = 1; break;
        case Qt::MidButton: button = 2; break;
        case Qt::RightButton: button = 3; break;
        case Qt::NoButton: button = 0; break;
        default: button = 0; break;
    }
    h->setKeyboardModifiers( event );
    QPointF pos = h->mousePoint(event);
    h->view->getEventQueue()->mouseDoubleButtonPress(pos.x(), pos.y(), button);
}

void OSGViewport::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "mouse press:" << event;
    int button = 0;
    switch ( event->button() )
    {
        case Qt::LeftButton: button = 1; break;
        case Qt::MidButton: button = 2; break;
        case Qt::RightButton: button = 3; break;
        case Qt::NoButton: button = 0; break;
        default: button = 0; break;
    }
    h->setKeyboardModifiers( event );
    QPointF pos = h->mousePoint(event);
    //qDebug() << "   pos:" << pos;
    h->view->getEventQueue()->mouseButtonPress(pos.x(), pos.y(), button);
}

void OSGViewport::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "mouse move:" << event;
    h->setKeyboardModifiers( event );
    QPointF pos = h->mousePoint(event);
    h->view->getEventQueue()->mouseMotion(pos.x(), pos.y());
}

void OSGViewport::mouseReleaseEvent(QMouseEvent *event)
{
    int button = 0;
    switch ( event->button() )
    {
        case Qt::LeftButton: button = 1; break;
        case Qt::MidButton: button = 2; break;
        case Qt::RightButton: button = 3; break;
        case Qt::NoButton: button = 0; break;
        default: button = 0; break;
    }
    h->setKeyboardModifiers( event );
    QPointF pos = h->mousePoint(event);
    h->view->getEventQueue()->mouseButtonRelease(pos.x(), pos.y(), button);
}

void OSGViewport::wheelEvent(QWheelEvent *event)
{
    //qDebug() << "wheel:" << event;
    h->view->getEventQueue()->mouseScroll(
                event->orientation() == Qt::Vertical ?
                    (event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN) :
                    (event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_LEFT : osgGA::GUIEventAdapter::SCROLL_RIGHT) );

}

void OSGViewport::keyPressEvent(QKeyEvent *event)
{
    h->setKeyboardModifiers( event );
    int value = h->keyMap.remapKey( event );
    h->view->getEventQueue()->keyPress( value );

    // this passes the event to the regular Qt key event processing,
    // among others, it closes popup windows on ESC and forwards the event to the parent widgets
    // TODO implement
//    if( _forwardKeyEvents )
//        inherited::keyPressEvent( event );
}

void OSGViewport::keyReleaseEvent(QKeyEvent *event)
{
    if( event->isAutoRepeat() )
    {
        event->ignore();
    }
    else
    {
        h->setKeyboardModifiers( event );
        int value = h->keyMap.remapKey( event );
        h->view->getEventQueue()->keyRelease( value );
    }

    // this passes the event to the regular Qt key event processing,
    // among others, it closes popup windows on ESC and forwards the event to the parent widgets
    // TODO implement
//    if( _forwardKeyEvents )
//        inherited::keyReleaseEvent( event );
}

QSGNode *OSGViewport::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    if (oldNode && oldNode != h->textureNode) {
        delete oldNode;
    }
    Q_UNUSED(updatePaintNodeData);

    return h->textureNode;
}

osgViewer::View *OSGViewport::view()
{
    //qDebug()<<Q_FUNC_INFO<<"get view";
    return h->view.get();
}

void OSGViewport::viewerDrawsFrame(AerogisViewer *v)
{
   //qDebug()<<Q_FUNC_INFO<<"draw frame";
   v->frame();
}

void OSGViewport::viewerDrawnFrame()
{
   // do nothing
}

} // namespace osgQtQuick

#include "OSGViewport.moc"
