/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*                                                                                    */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*                                                                                    */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*                                                                                    */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   */
/*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            */
/*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  */
/*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    */
/*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      */
/*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    */
/*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      */
/*                                                                                    */
/**************************************************************************************/

#ifndef AdjacencyExtractor_INCLUDE_ONCE
#define AdjacencyExtractor_INCLUDE_ONCE

#include <vlGraphics/link_config.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlCore/Time.hpp>

namespace vl
{
  class AdjacencyExtractor {
  private:
      struct SEdge {
        u32 A,B,O;

        SEdge() {
          A = B = O = 0xFFFFFFFF;
        }

        SEdge(u32 a, u32 b, u32 o) {
          A = a;
          B = b;
          O = o;
        }

        bool isNull() const {
          return A == 0xFFFFFFFF;
        }

        u64 id() const {
          return (u64)A | ( (u64)B << 32 );
        }

        u64 other() const {
          return (u64)B | ( (u64)A << 32 );
        }

        bool operator<( const SEdge& other ) const {
          return id() < other.id();
        }

        bool operator==( const SEdge& other ) const {
          return id() == other.id();
        }
      };

      struct STriangle {
        SEdge edge[3];
        STriangle( u32 a, u32 b, u32 c ) {
          edge[0] = SEdge(a,b,c);
          edge[1] = SEdge(b,c,a);
          edge[2] = SEdge(c,a,b);
        }
      };

      /** How many collisions can SEdgeMap handle before falling back to std::map. */
      static const int VL_SEdgeMap_SLOTS = 4;

      struct SEdgeMapSlots {
        SEdge slot[ VL_SEdgeMap_SLOTS ];
      };

      /** Basic hash-table implementation that falls back to std::map when too many collisions happen. */
      struct SEdgeMap {
        std::map<u64, SEdge> edge_map;
        std::vector<SEdgeMapSlots> cache;
        int cache_hits;
        int edge_count;

        size_t memoryUsed() const {
          return cache.size() * sizeof( SEdgeMapSlots ) + edge_map.size() * ( sizeof( SEdge ) + sizeof( u64 ) );
        }

        SEdgeMap( int size = 104729 ) {
          cache.resize( size );
          cache_hits = 0;
          edge_count = 0;
        }

        void put(u64 uid, const SEdge& edge) {
          ++edge_count;
          VL_CHECK( ! edge.isNull() );
          u64 p = uid % cache.size();
          for( int i = 0; i < VL_SEdgeMap_SLOTS; ++i ) {
            if ( cache[ p ].slot[ i ].isNull() ) {
              cache[ p ].slot[ i ] = edge;
              VL_CHECK( ! cache[ p ].slot[ i ].isNull() );
              ++cache_hits;
              return;
            }
          }
          edge_map[ uid ] = edge;
        }

        bool get(u64 uid, SEdge& edge_out) {
          edge_out = SEdge();
          VL_CHECK( edge_out.isNull() );
          u64 p = uid % cache.size();
          for( int i = 0; i < VL_SEdgeMap_SLOTS; ++i ) {
            if ( ! cache[ p ].slot[ i ].isNull() && cache[ p ].slot[ i ].id() == uid ) {
              edge_out = cache[ p ].slot[ i ];
              return true;
            }
          }

          std::map<u64, SEdge>::iterator it = edge_map.find( uid );
          if ( it != edge_map.end() ) {
            edge_out = it->second;
            return true;
          } else {
            return false;
          }
        }
      };

      /** Implements a very fast non-resizing hash-table with a finite max number of elements (see constructor).
        * Uses half the memory as SEdgeMap and is up to 2x as fast.
        * Does not use std::map.
        * Does not implement element deletion (not difficult to do so though).
        * Does not implement hash-table resizing (doable but very expensive).
        * Does not support and arbitrary number of elements (doable using maps as fallback or hash-table resizing).
        */
      class SEdgeMapFast {
        struct SEdgeSlot {
          SEdge edge;
          SEdgeSlot* next;
          SEdgeSlot() {
            next = NULL;
          }
        };

      private:
        std::vector<SEdgeSlot> cache;
        std::vector<SEdgeSlot> free_slots;
        SEdgeSlot* free_slots_head;

      public:
        int cache_hits;
        int edge_count;
        int free_slots_used;
        int max_size;

      public:
        /** Maximum number of elements that can be stored. */
        SEdgeMapFast( int size = 104729 ) {
          max_size = size;
          cache_hits = 0;
          edge_count = 0;
          free_slots_used = 0;
          cache.resize( size );
          free_slots.resize( size );
          free_slots_head = &free_slots[0];
          for( int i = 0; i < free_slots.size() - 1; ++i ) {
            free_slots[i].next = &free_slots[i + 1];
          }
        }

        size_t memoryUsed() const {
          return cache.size() * sizeof( SEdgeSlot ) + free_slots.size() * sizeof( SEdgeSlot );
        }

        void put(u64 uid, const SEdge& edge) {
          ++edge_count;
          u64 p = uid % cache.size();

          // Find and replace
          SEdgeSlot* slot = &cache[ p ];
          SEdgeSlot* last = NULL;
          for( ; slot != NULL; last = slot, slot = slot->next )
          {
            // first is free or found the edge
            if( slot->edge.isNull() || slot->edge.id() == edge.id() ) {
              slot->edge = edge;
              if ( last == NULL) {
                ++cache_hits;
              }
              return;
            }
          }

          // Out of memory: increase SEdgeMapFast size when constructing it.
          VL_CHECK( free_slots_head );

          // Get a free slot and append it to the end of the list
          SEdgeSlot* free_slot = free_slots_head;
          free_slots_head = free_slots_head->next;
          free_slot->next = NULL;
          free_slot->edge = edge;
          last->next = free_slot;
          ++free_slots_used;
        }

        bool get(u64 uid, SEdge& edge_out) {
          edge_out = SEdge();
          u64 p = uid % cache.size();
          // find
          for( SEdgeSlot* slot = &cache[ p ]; slot != NULL; slot = slot->next ) {
            if ( ! slot->edge.isNull() && slot->edge.id() == uid ) {
              edge_out = slot->edge;
              return true;
            }
          }
          return false;
        }
      };

  public:
    static ref< Geometry > extract( Geometry* geom ) {

      #ifndef NDEBUG
        float t0 = Time::currentTime();
      #endif

      ref< Geometry > geom_adj = new Geometry;
      geom_adj->setVertexArray( geom->vertexArray() );
      geom_adj->setNormalArray( geom->normalArray() );

      int total_triangles = 0;
      for( int idc = 0; idc < geom->drawCalls().size(); ++idc ) {
        int triangle_count = 0;
        const DrawCall* dc = geom->drawCalls()[ idc ].get();
        int indices = dc->countIndices();
        SEdgeMapFast edge_map( indices );
        // SEdgeMap edge_map( indices * 2 );

        if ( dc->primitiveType() == vl::PT_LINES_ADJACENCY      ||
             dc->primitiveType() == vl::PT_LINE_STRIP_ADJACENCY ||
             dc->primitiveType() == vl::PT_TRIANGLES_ADJACENCY  ||
             dc->primitiveType() == vl::PT_TRIANGLE_STRIP_ADJACENCY ) {
          Log::error( "AdjacencyExtractor::extract(): geometry has already adjacency information." );
          return NULL;
        }

        for( TriangleIterator trit = dc->triangleIterator(); trit.hasNext(); trit.next() )
        {
          u32 a = trit.a();
          u32 b = trit.b();
          u32 c = trit.c();
          STriangle triangle( a, b, c );
          edge_map.put( triangle.edge[0].id(), triangle.edge[0] );
          edge_map.put( triangle.edge[1].id(), triangle.edge[1] );
          edge_map.put( triangle.edge[2].id(), triangle.edge[2] );
          ++triangle_count;
        }
        total_triangles += triangle_count;

        ref< DrawElementsUInt > dc_adj = new DrawElementsUInt( PT_TRIANGLES_ADJACENCY );
        geom_adj->drawCalls().push_back( dc_adj.get() );
        dc_adj->indexBuffer()->resize( triangle_count * 6 );
        GLuint* P = dc_adj->indexBuffer()->begin();

        for( TriangleIterator trit = dc->triangleIterator(); trit.hasNext(); trit.next(), P += 6 )
        {
          u32 a = trit.a();
          u32 b = trit.b();
          u32 c = trit.c();

          // NOTE: degenerate edges are important for border detection.

          P[0] = a;
          P[1] = a; // degenerate
          P[2] = b;
          P[3] = b; // degenerate
          P[4] = c;
          P[5] = c; // degenerate

          STriangle triangle( a, b, c );
          SEdge edge;
          if ( edge_map.get( triangle.edge[0].other(), edge ) ) {
            VL_CHECK( ! edge.isNull() );
            P[1] = edge.O;
          }
          if ( edge_map.get( triangle.edge[1].other(), edge ) ) {
            VL_CHECK( ! edge.isNull() );
            P[3] = edge.O;
          }
          if ( edge_map.get( triangle.edge[2].other(), edge ) ) {
            VL_CHECK( ! edge.isNull() );
            P[5] = edge.O;
          }
        }

        #ifndef NDEBUG
          printf("EdgeMap: edge-count: %d, cache-hits: %d (%.1f%%), cache MB: %.1f\n",
                 edge_map.edge_count, edge_map.cache_hits, 100.0f * edge_map.cache_hits / edge_map.edge_count,
                 edge_map.memoryUsed() / (1024.0 * 1024.0) );
        #endif
      }

      #ifndef NDEBUG
        float secs = Time::currentTime() - t0;
        printf( "Adjacency Time: %.1fs, %.1fKtri/sec (%d)\n", secs, total_triangles / secs / 1000.0f, total_triangles );
      #endif

      return geom_adj;
    }
  };
}

#endif
