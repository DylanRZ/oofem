/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2013   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "enrichmentfrontextend.h"
#include "dynamicinputrecord.h"
#include "classfactory.h"
#include "xfem/xfemmanager.h"
#include "domain.h"
#include "connectivitytable.h"

namespace oofem {
REGISTER_EnrichmentFront(EnrFrontExtend)

		void EnrFrontExtend :: MarkNodesAsFront(std :: vector< int > &ioNodeEnrMarker, XfemManager &ixFemMan, const std :: vector< double > &iLevelSetNormalDir, const std :: vector< double > &iLevelSetTangDir, const std :: vector< TipInfo > &iTipInfo)
		{
		    // Extend the set of enriched nodes as follows:
		    // If any node of the neighboring elements is enriched, the current node is also enriched.

		    Domain &d = * ( ixFemMan.giveDomain() );

		    // Loop over all nodes
		    int nNodes = d.giveNumberOfDofManagers();

		    std :: vector< int >newEnrNodes;
		    for ( int i = 1; i <= nNodes; i++ ) {
		        // Check if the node is already enriched
		        bool alreadyEnr = ( ioNodeEnrMarker [ i - 1 ] > 0 );

		#if defined ( ENABLE_XFEM_CPP11 )
		        auto begin      = newEnrNodes.begin();
		        auto end        = newEnrNodes.end();
		#else
		        std :: vector< int > :: const_iterator begin  = newEnrNodes.begin();
		        std :: vector< int > :: const_iterator end    = newEnrNodes.end();
		#endif
		        if ( std :: binary_search(begin, end, i) ) {
		            alreadyEnr = true;
		        }


		        if ( !alreadyEnr ) {
		            bool goOn = true;

		            // Loop over neighbors
		            const IntArray &neigh = * ( d.giveConnectivityTable()->giveDofManConnectivityArray(i) );
		            for ( int j = 1; j <= neigh.giveSize(); j++ ) {
		                if ( !goOn ) {
		                    break;
		                }

		                Element &el = * ( d.giveElement( neigh.at(j) ) );

		                // Loop over neighbor element nodes
		                for ( int k = 1; k <= el.giveNumberOfDofManagers(); k++ ) {
		                    int kGlob = el.giveDofManager(k)->giveGlobalNumber();
		                    if ( iLevelSetNormalDir [ kGlob - 1 ] < 0 ) {
		                        newEnrNodes.push_back(i);
		                        goOn = false;
		                        break;
		                    }
		                }
		            }
		        }
		    }


		    // Mark the new nodes to be enriched
		    for ( int i = 0; i < int ( newEnrNodes.size() ); i++ ) {
		        ioNodeEnrMarker [ newEnrNodes [ i ] - 1 ] = 1;
		    }
		}

		void EnrFrontExtend :: giveInputRecord(DynamicInputRecord &input)
		{
		    int number = 1;
		    input.setRecordKeywordField(this->giveInputRecordName(), number);
		}


} // end namespace oofem
