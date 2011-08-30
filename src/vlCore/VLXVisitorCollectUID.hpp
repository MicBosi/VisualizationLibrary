/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
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

#ifndef VLXVisitorCollectUID_INCLUDE_ONCE
#define VLXVisitorCollectUID_INCLUDE_ONCE

#include <vlCore/VLXVisitor.hpp>
#include <vlCore/VLXValue.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // VLXVisitorCollectUID:
  // Sets to #NULL the UID of those objects that are not referenced by anybody.
  // Useful before exporting.
  //-----------------------------------------------------------------------------
  class VLXVisitorCollectUID: public VLXVisitor
  {
  public:
    VLXVisitorCollectUID(): mUIDSet(NULL) {}

    virtual void visitStructure(VLXStructure* obj)
    {
      if(!obj->uid().empty() && obj->uid() != "#NULL")
        (*mUIDSet)[obj->uid()]++;

      if (isVisited(obj))
        return;

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        VLXStructure::Value& keyval = obj->value()[i];
        if (keyval.value().type() == VLXValue::Structure)
          keyval.value().getStructure()->acceptVisitor(this);
        else
        if (keyval.value().type() == VLXValue::List)
          keyval.value().getList()->acceptVisitor(this);
        else
        /*
        if (keyval.value().type() == VLXValue::ArrayUID)
          keyval.value().getArrayUID()->acceptVisitor(this);
        else
        */
        if (keyval.value().type() == VLXValue::UID)
          (*mUIDSet)[keyval.value().getUID()]++;
      }
    }

    virtual void visitList(VLXList* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("VLXVisitorCollectUID: cycle detected on VLXList.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == VLXValue::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        if (list->value()[i].type() == VLXValue::List)
          list->value()[i].getList()->acceptVisitor(this);
        else
        /*
        if (list->value()[i].type() == VLXValue::ArrayUID)
          list->value()[i].getArrayUID()->acceptVisitor(this);
        else
        */
        if (list->value()[i].type() == VLXValue::UID)
          (*mUIDSet)[list->value()[i].getUID()]++;
      }
    }

    /*
    virtual void visitArray(VLXArrayString*)  {}

    virtual void visitArray(VLXArrayUID* arr)
    {
      // retrieves the assigned Structure
      for(size_t i=0 ;i<arr->value().size(); ++i)
        (*mUIDSet)[arr->value()[i].uid()]++;
    }

    virtual void visitArray(VLXArrayIdentifier*) {}
    */

    virtual void visitArray(VLXArrayInteger*)  {}

    virtual void visitArray(VLXArrayReal*)  {}

    void setUIDSet(std::map< std::string, int >* uids) { mUIDSet = uids; }

    std::map< std::string, int >* uidSet() { return mUIDSet; }

    const std::map< std::string, int >* uidSet() const { return mUIDSet; }

  private:
    std::map< std::string, int >* mUIDSet;
  };
}

#endif
