/*
 *      set_cover.hpp
 *      
 *      Copyright 2009 David Gleich <dgleich@stanford.edu>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

/**
 * @file set_cover.hpp
 * @author David F. Gleich
 * @brief Approximation algorithms for the set covering problem.
 */

namespace approx {
    
    /** Approximate the mininum cost set cover with a greedy algorithm.
     * 
     * Mathematically, the minimum cost set cover problem is
     *   min  sum c_i x_i
     *   s.t. union over non-zero x_i of S_i = U (universe of items)
     *        x_i is binary
     * 
     * @param n the number of items in the universe
     * @param set_of_sets a collection of itemsets that define the set 
     *  covering problem
     *      size_t set_of_sets.size()
     *      set_of_sets[i].size() for 0 < i < set_of_sets.size()
     *      set_of_sets[i][j] is an item 0 < j < set_of_sets[i].size()
     * @param cost the cost of a set
     *      cost[i] for 0 < i < set_of_sets.size()
     *      CostVector::value_type
     * @param item_index a mapping between the items and a linear (0-n-1) 
     *      item_index[set_of_sets[i][j]] must be in [0,n-1] for all valid i, j
     * @param item_cost_func a functor to compute the cost of an item 
     * 
     * @example 
     *  size_t n=5;
     *  std::vector<double> cost(3);
     *  std::vector< std::vector< int > > set_of_sets(3);
     *  set_of_sets[0].push_back(0,1,2)
     *  set_of_sets[1].push_back(1,2,3)
     *  set_of_sets[2].push_back(0,4)
     *  cost[0] = 1.0; cost[1]
     *  greedy_set_cover
     */
    
    template < typename SizeType, typename SetOfSets, typename ItemIndexMap, 
        typename ItemCostFunc >
    void greedy_set_cover(
            SizeType n,
            SetOfSets& set_of_sets,
            CostVector cost,
            ItemIndexMap item_index,
            ItemCostFunc item_cost_func
            )
    {
        // needs priority queue initialized 
        // 
/*
 * def find_greedy_cover(self):
        GreedyCover = CollectionOfSets() 
        GreedyCover.set_itemvaluefunc(self.itemvaluefunc)
        GreedyCover.set_costfunc(self.costfunc)

        #keep a local copy of the itemsets' values, which we will decrement.
        currentvalues = list(self.value)
        
        PQ = priorityDictionary()
        uncovered = set()
        for (i,S) in enumerate(self.itemsets):
            uncovered.update(S)
            if self.value[i] > 0:
                PQ[i] = self.cost[i] / float(self.value[i])
            else:
                PQ[i] = LARGEINT
        
        while len(uncovered) > 0:
            #get set with smallest cost-per-value from priority queue
            i = PQ.smallest()
            best_set = self.itemsets[i]
            print 'cost-per-value = %s'%PQ.pop(i)

            GreedyCover.add_set(self.itemsets[i],self.itemsetnames[i],cost=self.cost[i])

            changeditemsets = list()
            for item in best_set:
                if item in uncovered:
                    uncovered.remove(item)
                    for j in self.index[item]:
                        #decrease value and value in the priority queue
                        assert item in self.itemsets[j]
                        currentvalues[j] -= self.itemvaluefunc(item)
                        currentvalue = currentvalues[j]
                        if currentvalue > 0:
                            PQ[j] = self.cost[j] / float(currentvalue)
                        else:
                            PQ[j] = LARGEINT

        return GreedyCover
        * /        
    }
        
        
    
}; // approx namespace
