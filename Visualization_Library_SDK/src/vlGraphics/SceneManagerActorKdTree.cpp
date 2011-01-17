/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
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

#include "vlCore/SceneManagerKdTree.hpp"
#include <set>
using namespace vl;

//-----------------------------------------------------------------------------
// SceneManagerKdTree
//-----------------------------------------------------------------------------
//void SceneManagerKdTree::cullKdTree(KdTree* tree)
//{
//  if ( !frustumCullingEnabled() || !mCamera->frustumCull( tree->aabb() ) )
//  {
//    for(int i=0; i<(int)tree->actorCount(); ++i)
//      if ( tree->actor(i)->enabled() )
//        mActorQueue->push_back( tree->actor(i) );
//
//    if (tree->childN())
//      cullKdTree( tree->childN() );
//
//    if (tree->childP())
//      cullKdTree( tree->childP() );
//  }
//}
//-----------------------------------------------------------------------------
//void SceneManagerKdTree::updateTransforms(Camera* camera, ActorQueue& actors)
//{
//  // finds the root transforms
//
//  std::set< Transform* > root_transforms;
//  for(int i=0; i<(int)actors.size(); ++i)
//  {
//    for( Transform* root = actors[i]->transform(); root; root = root->parent() )
//      if ( !root->parent() )
//        root_transforms.insert(root);
//  }
//
//  // setup the matrices
//
//  std::set< Transform* >::iterator tra = root_transforms.begin();
//  while( tra != root_transforms.end() )
//  {
//    (*tra)->computeWorldMatrixRecursive(camera);
//    ++tra;
//  }
//}
//-----------------------------------------------------------------------------
//void SceneManagerKdTree::appendVisibleActors(ActorQueue* queue, const Camera* camera)
//{
//  mActorQueue = queue;
//  mCamera     = camera;
//  cullKdTree(mKdTree.get());
//}
//-----------------------------------------------------------------------------
//void SceneManagerKdTree::appendActors(ActorQueue* queue)
//{
//  ActorQueue actors;
//
//  mKdTree->getActorsRecursive(actors);
//
//  for(int i=0; i<(int)actors.size(); ++i)
//    queue->push_back(actors[i].get());
//}
//-----------------------------------------------------------------------------
