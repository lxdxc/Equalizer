
/* Copyright (c) 2009, Stefan Eilemann <eile@equalizergraphics.com> 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "canvas.h"

#include "canvasVisitor.h"
#include "config.h"
#include "global.h"
#include "layout.h"
#include "nodeFactory.h"
#include "segment.h"

#include <eq/net/dataIStream.h>
#include <eq/net/dataOStream.h>

namespace eq
{

Canvas::Canvas()
        : _config( 0 )
        , _layout( 0 )
{
}

Canvas::~Canvas()
{
    EQASSERT( !_config );
}

void Canvas::serialize( net::DataOStream& os, const uint64_t dirtyBits )
{
    Frustum::serialize( os, dirtyBits );

    if( dirtyBits & DIRTY_LAYOUT )
    {
        if( _layout )
        {
            EQASSERT( _layout->getID() != EQ_ID_INVALID );
            os << _layout->getID();
        }
        else
            os << EQ_ID_INVALID;
    }
    EQASSERT( !(dirtyBits & DIRTY_SEGMENTS ));
}

void Canvas::deserialize( net::DataIStream& is, const uint64_t dirtyBits )
{
    Frustum::deserialize( is, dirtyBits );

    if( dirtyBits & DIRTY_LAYOUT )
    {
        uint32_t id;
        is >> id;
        if( id == EQ_ID_INVALID )
            _layout = 0;
        else
        {
            EQASSERT( _config );
            _layout = _config->findLayout( id );
            EQASSERT( _layout );
        }
    }

    if( dirtyBits & DIRTY_SEGMENTS )
    {
        EQASSERT( _segments.empty( ));
        EQASSERT( _config );

        NodeFactory* nodeFactory = Global::getNodeFactory();
        uint32_t id;
        for( is >> id; id != EQ_ID_INVALID; is >> id )
        {
            Segment* segment = nodeFactory->createSegment();
            segment->_canvas = this;
            _segments.push_back( segment );

            _config->mapObject( segment, id );
            // RO, don't: segment->becomeMaster();
        }
    }
}

void Canvas::deregister()
{
    EQASSERT( _config );
    EQASSERT( isMaster( ));
    NodeFactory* nodeFactory = Global::getNodeFactory();

    for( SegmentVector::const_iterator i = _segments.begin(); 
         i != _segments.end(); ++i )
    {
        Segment* segment = *i;
        EQASSERT( segment->getID() != EQ_ID_INVALID );
        EQASSERT( !segment->isMaster( ));

        _config->deregisterObject( segment );
        segment->_canvas = 0;
        nodeFactory->releaseSegment( segment );
    }
    
    _segments.clear();
    _config->deregisterObject( this );
}

void Canvas::useLayout( Layout* layout )
{
    if( _layout == layout )
        return;

    _layout = layout;
    setDirty( DIRTY_LAYOUT );
}

namespace
{
template< class C, class V >
VisitorResult _accept( C* canvas, V& visitor )
{
    VisitorResult result = visitor.visitPre( canvas );
    if( result != TRAVERSE_CONTINUE )
        return result;

    const SegmentVector& segments = canvas->getSegments();
    for( SegmentVector::const_iterator i = segments.begin(); 
         i != segments.end(); ++i )
    {
        switch( (*i)->accept( visitor ))
        {
            case TRAVERSE_TERMINATE:
                return TRAVERSE_TERMINATE;

            case TRAVERSE_PRUNE:
                result = TRAVERSE_PRUNE;
                break;
                
            case TRAVERSE_CONTINUE:
            default:
                break;
        }
    }

    switch( visitor.visitPost( canvas ))
    {
        case TRAVERSE_TERMINATE:
            return TRAVERSE_TERMINATE;

        case TRAVERSE_PRUNE:
            return TRAVERSE_PRUNE;
                
        case TRAVERSE_CONTINUE:
        default:
            break;
    }

    return result;
}
}

VisitorResult Canvas::accept( CanvasVisitor& visitor )
{
    return _accept( this, visitor );
}

}
