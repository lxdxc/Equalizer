/* Copyright (c) 2006-2007, Stefan Eilemann <eile@equalizergraphics.com> 
   All rights reserved. */

#include "frame.h"

#include "frameData.h"
#include "packets.h"

#include <eq/net/session.h>

using namespace std;

namespace eq
{

Frame::Frame()
        : _frameData( 0 )
{
    setInstanceData( &_data, sizeof( Data ));
}

uint32_t Frame::getBuffers() const
{
    EQASSERT( _frameData );
    return _frameData->getBuffers();
}

const Pixel& Frame::getPixel() const
{
    EQASSERT( _frameData );
    return _frameData->getPixel();
}

const Range& Frame::getRange() const
{
    EQASSERT( _frameData );
    return _frameData->getRange();
}

const std::vector<Image*>& Frame::getImages() const
{
    EQASSERT( _frameData );
    return _frameData->getImages();
}

void Frame::startReadback() 
{
    EQASSERT( _frameData );
    _frameData->startReadback( *this );
}

void Frame::syncReadback() 
{
    EQASSERT( _frameData );
    _frameData->syncReadback();
}

void Frame::transmit( eqBase::RefPtr<eqNet::Node> toNode )
{
    EQASSERT( _frameData );
    _frameData->transmit( toNode );
}

bool Frame::isReady() const
{
    EQASSERT( _frameData );
    return _frameData->isReady();
}

void Frame::waitReady() const
{
    EQASSERT( _frameData );
    _frameData->waitReady();
}

void Frame::disableBuffer( const Buffer buffer )
{
    EQASSERT( _frameData );
    _frameData->disableBuffer( buffer );
}


void Frame::addListener( eqBase::Monitor<uint32_t>& listener )
{
    EQASSERT( _frameData );
    _frameData->addListener( listener );
}

void Frame::removeListener( eqBase::Monitor<uint32_t>& listener )
{
    EQASSERT( _frameData );
    _frameData->removeListener( listener );
}

EQ_EXPORT std::ostream& operator << ( std::ostream& os, 
                                      const Frame::Buffer buffer )
{
    if( buffer == Frame::BUFFER_NONE )
        os << "none ";
    else if( buffer & Frame::BUFFER_UNDEFINED )
        os << "undefined ";
    else
    {
        if( buffer & Frame::BUFFER_COLOR )
            os << "color ";
        if( buffer & Frame::BUFFER_DEPTH )
            os << "depth ";
    }

    return os;
}

}
