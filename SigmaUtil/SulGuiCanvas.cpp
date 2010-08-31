// SulGuiCanvas.cpp

#include "stdafx.h"
#include "SulGuiCanvas.h"
#include "SulGuiComp.h"
#include "SulShaderGuiFrame.h"
#include "SulGuiEventHandler.h"
#include <osgManipulator/Selection>

CSulGuiCanvas::CSulGuiCanvas( const CSulString& sCompName ) :
CSulGuiComp( sCompName )
{
	initConstructor();
}

CSulGuiCanvas::CSulGuiCanvas( const CSulString& sCompName, float x, float y ) :
CSulGuiComp( sCompName, x, y )
{
	initConstructor();
}

CSulGuiCanvas::CSulGuiCanvas( const CSulString& sCompName, float x, float y, float w, float h ) :
CSulGuiComp( sCompName, x, y )
{
	initConstructor();
	m_w = w;
	m_h = h;
}

void CSulGuiCanvas::initConstructor()
{
	m_dragDragging = false;
	m_dragAllowed = false;
	m_dragMinX = 0.0f;
	m_dragMaxX = 10000000.0f;		
	m_dragMinY = 0.0f;
	m_dragMaxY = 10000000.0f;
	m_bMouseHover = false;
	m_bShowCanvas	= true;
}

void CSulGuiCanvas::setupTheme( CSulGuiThemeXml* pThemeXml )
{
	CSulGuiComp::setupTheme( pThemeXml );

	m_w = getThemeValue( "w" ).asFloat();
	m_h = getThemeValue( "h" ).asFloat();
}

void CSulGuiCanvas::setupAttr( CSulXmlAttr* pAttr )
{
	CSulGuiComp::setupAttr( pAttr );

	if ( pAttr->exist( "w" ) )				m_w = pAttr->get( "w" ).asFloat();
	if ( pAttr->exist( "h" ) )				m_h = pAttr->get( "h" ).asFloat();
	if ( pAttr->exist( "img" ) )			m_img = pAttr->get( "img" );
	if ( pAttr->exist( "show_canvas" ) )	m_bShowCanvas = pAttr->get( "show_canvas" ).asBool();
}

void CSulGuiCanvas::setupEventHandler( CSulGuiEventHandler* pEventHandler )
{
	CSulGuiComp::setupEventHandler( pEventHandler );

	pEventHandler->signalMouseMove.connect( this, &CSulGuiCanvas::onMouseMove );
	pEventHandler->signalMouseDrag.connect( this, &CSulGuiCanvas::onMouseDrag );
	pEventHandler->signalMousePush.connect( this, &CSulGuiCanvas::onMousePush );
	pEventHandler->signalMouseRelease.connect( this, &CSulGuiCanvas::onMouseRelease );
	pEventHandler->signalViewResize.connect( this, &CSulGuiCanvas::onViewResize );
}

void CSulGuiCanvas::init()
{
	CSulGuiComp::init();

	float w = getW();

	m_rQuad = new CSulGeomQuad(
		osg::Vec3( w/2.0f, getH()/2.0f, 0.0f ),
		w, getH() );
	m_rQuad->createUV();
	m_rGeodeQuad = new osg::Geode;
	m_rGeodeQuad->addDrawable( m_rQuad->getDrawable() );

	osg::MatrixTransform::addChild( m_rGeodeQuad );

	// add a shader (perhaps we should move this to xml manager so we only have one shader)
	new CSulShaderGuiFrame( m_rGeodeQuad );

	m_rGeodeQuad->getOrCreateStateSet()->addUniform( m_uniformUseTexture = new osg::Uniform( "use_texture", 0 ) );
	m_rGeodeQuad->getOrCreateStateSet()->addUniform( new osg::Uniform( "cover", 0 ) );
	m_rGeodeQuad->getOrCreateStateSet()->addUniform( m_uniformW = new osg::Uniform( "w", getW() ) );
	m_rGeodeQuad->getOrCreateStateSet()->addUniform( m_uniformH = new osg::Uniform( "h", getH() ) );
	m_rGeodeQuad->getOrCreateStateSet()->addUniform( new osg::Uniform( "border", 2.0f ) );
	m_rGeodeQuad->getOrCreateStateSet()->addUniform( m_uniformBgColor = new osg::Uniform( "bg_color", osg::Vec4(0,0,0,0.2f) ) );
	m_rGeodeQuad->getOrCreateStateSet()->addUniform( m_uniformBorderColor = new osg::Uniform( "border_color", osg::Vec4(0,0,1,1) ) );

	if ( !m_img.empty() )
	{
		m_rQuad->setTexture( m_img, 0 );
		m_uniformUseTexture->set( 1 );
		
		if ( m_w==0.0f || m_h==0.0f )
		{
			osg::Image* pImage = m_rQuad->getImage();
			setWH( pImage->s(), pImage->t() );
		}
	}

	showCanvas( m_bShowCanvas );
}

void CSulGuiCanvas::setDraggable( bool bDraggable )
{
	m_dragAllowed = bDraggable;
}

void CSulGuiCanvas::showCanvas( bool bShow )
{
	m_rGeodeQuad->setNodeMask( bShow?0xFFFFFFFF:0 );
}

void CSulGuiCanvas::setBgColor( const osg::Vec4& c )
{
	m_uniformBgColor->set( c );
}

void CSulGuiCanvas::setBorderColor( const osg::Vec4& c )
{
	m_uniformBorderColor->set( c );
}

void CSulGuiCanvas::setWH( float w, float h )
{
	m_w = w;
	m_h = h;

	if ( m_rQuad.valid() )
	{
		m_rQuad->setCenter( osg::Vec3( w/2.0f, h/2.0f, 0.0f ) );
		m_rQuad->setWidth( m_w );
		m_rQuad->setHeight( m_h );
		m_uniformW->set( m_w );
		m_uniformH->set( m_h );
	}
}

void CSulGuiCanvas::setW( float w )
{
	m_w = w;
	if ( m_rQuad.valid() )
	{
		m_rQuad->setCenter( osg::Vec3( m_w/2.0f, m_h/2.0f, 0.0f ) );
		m_rQuad->setWidth( m_w );
		m_uniformW->set( m_w );
	}
}

void CSulGuiCanvas::setH( float h )
{
	m_h = h;
	if ( m_rQuad.valid() )
	{
		m_rQuad->setCenter( osg::Vec3( m_w/2.0f, m_h/2.0f, 0.0f ) );
		m_rQuad->setHeight( m_h );
		m_uniformH->set( m_h );
	}
}

float CSulGuiCanvas::getW()
{
	return m_w;
}

float CSulGuiCanvas::getH()
{
	return m_h;
}

bool CSulGuiCanvas::isInside( float x, float y )
{
	return ( x>0 && y>0 && x<getW() && y<getH() )?true:false;
}


void CSulGuiCanvas::setMouseRelease( bool bInside )
{
}

void CSulGuiCanvas::allowDrag( float minX, float maxX, float minY, float maxY )
{
	m_dragAllowed	= true;
	m_dragMinX		= minX;
	m_dragMaxX		= maxX;
	m_dragMinY		= minY;
	m_dragMaxY		= maxY;
}


void CSulGuiCanvas::onMousePush( float x, float y )
{
	// calc local positions
	osg::NodePath pathToRoot;
	osgManipulator::computeNodePathToRoot( *this, pathToRoot );
	osg::Matrix m = osg::computeLocalToWorld( pathToRoot );
	float local_x = x-m.getTrans().x();
	float local_y = y-m.getTrans().y();

	if ( m_dragAllowed && isInside( local_x, local_y ) )
	{
		m_dragOfsPos = osg::Vec2( local_x, local_y );
		m_dragDragging = true;
	}
}

void CSulGuiCanvas::onMouseRelease( float x, float y )
{
	// calc local positions
	osg::NodePath pathToRoot;
	osgManipulator::computeNodePathToRoot( *this, pathToRoot );
	osg::Matrix m = osg::computeLocalToWorld( pathToRoot );
	float local_x = x-m.getTrans().x();
	float local_y = y-m.getTrans().y();

	// check dragging
	if ( m_dragDragging )
	{
		m_dragDragging = false;
	}

	// something else...
	if ( isInside( local_x, local_y ) )
	{
		signalClicked( this );
		setMouseRelease( true );
	}
	else
	{
		setMouseRelease( false );
	}
}

void CSulGuiCanvas::onMouseMove( float x, float y )
{
	// calc local positions
	osg::NodePath pathToRoot;
	osgManipulator::computeNodePathToRoot( *this, pathToRoot );
	osg::Matrix m = osg::computeLocalToWorld( pathToRoot );
	float local_x = x-m.getTrans().x();
	float local_y = y-m.getTrans().y();

	// check hover
	if ( local_x>0 && local_y>0 && local_x<getW() && local_y<getH() )
	{
		if ( !m_bMouseHover )
		{
			m_bMouseHover = true;
			signalHover( true );
		}
	}
	else
	{
		if ( m_bMouseHover )
		{
			m_bMouseHover = false;
			signalHover( false );
		}
	}

}

void CSulGuiCanvas::onMouseDrag( float x, float y )
{
	// calc local positions
	osg::NodePath pathToRoot;
	osgManipulator::computeNodePathToRoot( *this, pathToRoot );
	osg::Matrix m = osg::computeLocalToWorld( pathToRoot );
	float local_x = x-m.getTrans().x();
	float local_y = y-m.getTrans().y();

	// check dragging
	if ( m_dragDragging )
	{
		osg::Vec2 d = osg::Vec2( local_x, local_y ) - m_dragOfsPos;
		osg::Vec2 pos = osg::Vec2( getX(), getY() ) + d;

		// check limits
		if ( pos.x()<m_dragMinX ) pos.x() = m_dragMinX;
		if ( pos.x()>m_dragMaxX ) pos.x() = m_dragMaxX;
		if ( pos.y()<m_dragMinY ) pos.y() = m_dragMinY;
		if ( pos.y()>m_dragMaxY ) pos.y() = m_dragMaxY;
		
		setXY( pos.x(), pos.y() );
	}
}

void CSulGuiCanvas::onViewResize( float w, float h  )
{
return;
	osg::Vec2 d = getNativeDimensions();

	float ww = m_w * (d.x()/w);
	float hh = m_h * (d.y()/h);

	m_rQuad->setCenter( osg::Vec3( ww/2.0f, hh/2.0f, 0.0f ) );

	m_rQuad->setWidth( ww );
	m_rQuad->setHeight( hh );

	//osg::notify(osg::NOTICE) << "ww= " << ww << "   hh= " << hh << std::endl;
	osg::notify(osg::NOTICE) << "m_w= " << m_w << "   m_h= " << m_h << std::endl;
}
