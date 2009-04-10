
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

#ifndef EQSERVER_SEGMENT_H
#define EQSERVER_SEGMENT_H

#include "paths.h"
#include "types.h"
#include "segmentVisitor.h"  // used in inline method

#include <eq/client/segment.h>      // base class
#include <eq/client/viewport.h>     // member

namespace eq
{
namespace server
{
    class Canvas;

    /**
     * The segment. @sa eq::Segment
     */
    class Segment : public eq::Segment
    {
    public:
        /** 
         * Constructs a new Segment.
         */
        Segment();

        /** Creates a new, deep copy of a segment. */
        Segment( const Segment& from, Config* config );

        /** Destruct this segment. */
        virtual ~Segment();

        /**
         * @name Data Access
         */
        //*{
        /** @return the config of this view. */
        Config* getConfig();

        /** @return the config of this view. */
        const Config* getConfig() const;

        /** @return the index path to this segment. */
        SegmentPath getPath() const;

        /** 
         * Set the channel of this segment.
         *
         * The channel defines the output area for this segment, typically a
         * rendering area covering a graphics card output.
         * 
         * @param channel the channel.
         */
        void setChannel( Channel* channel ) { _channel = channel; }

        /** Return the output channel of this segment. */
        Channel* getChannel()               { return _channel; }
        const Channel* getChannel() const   { return _channel; }

        /** 
         * Set the segment's viewport wrt its canvas.
         *
         * The viewport defines which 2D area of the canvas is covered by this
         * segment. Destination channels are created on the intersection of
         * segment viewports and the views of the layout used by the canvas.
         * 
         * @param vp the fractional viewport.
         */
        void setViewport( const eq::Viewport& vp );

        /** @return the segment's viewport. */
        const eq::Viewport& getViewport() const { return _vp; }

        /** Add a destination (View) channel. */
        void addDestinationChannel( Channel* channel );

        /** Remove a destination (View) channel. */
        bool removeDestinationChannel( Channel* channel );

        /** @return the vector of channels resulting from the segment/view
         *          intersection. */
        const ChannelVector& getDestinationChannels() const 
            { return _destinationChannels; }
        //*}
        
        /** Operations */
        //*{
        /** 
         * Traverse this segment using a segment visitor.
         * 
         * @param visitor the visitor.
         * @return the result of the visitor traversal.
         */
        VisitorResult accept( SegmentVisitor& visitor )
            { return visitor.visit( this ); }
        VisitorResult accept( ConstSegmentVisitor& visitor ) const
            { return visitor.visit( this ); }
        //*}

    protected:

    private:
        virtual void getInstanceData( net::DataOStream& os );

        /** The parent canvas. */
        Canvas* _canvas;
        friend class Canvas;

        /** The output channel of this segment. */
        Channel* _channel;

        /** The resulting destination channels from the view intersections. */
        ChannelVector _destinationChannels;

        union // placeholder for binary-compatible changes
        {
            char dummy[64];
        };

        /** Update the view (wall/projection). */
        void _updateView();
    };

    std::ostream& operator << ( std::ostream& os, const Segment* segment);
}
}
#endif // EQSERVER_SEGMENT_H
