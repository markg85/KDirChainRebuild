#pragma once
#ifndef ALTLRU_TYPES_HPP
#define ALTLRU_TYPES_HPP

#include <string>
#include <map>
#include <unordered_map>
#include <list>

template <typename T>
class AltLRU_CacheEntry;



template <typename T>
struct AltLRUType {
	typedef long altlru_key_add_id;

	typedef std::pair<std::string, altlru_key_add_id> altlru_map_pair;
	typedef AltLRU_CacheEntry<T>* altlru_map_pair_value;

	// Hashing functor for unordered_map
	typedef struct {
		inline long operator() (const altlru_map_pair &k) const {
			// DJB2 algorithm
			register long sum = k.second;
			register const char *s = k.first.c_str();
			while( (*s) != 0 ) {
				sum = ( (sum << 5) + sum) + *(s++);
			}
			return sum;
		}
	} AggregateKeyHash;

	/* a unordered_map cus it have search O(1) and have good iterator validity for altlru_list (if rehash wont happen) */
	typedef std::unordered_map< altlru_map_pair, altlru_map_pair_value, AggregateKeyHash > altlru_map;
	typedef typename altlru_map::iterator altlru_map_iter;

	/* a list cus is it have O(1) for insert in front/back and have good iterator validity for AltLRU_CacheEntry */
	typedef std::list< altlru_map_iter > altlru_list;

	/* These altlru_map and altlru_list will chain to each other so we can:
		- move item in altlru_list with O(1) when it is accesed by key of altlru_map
			to implement LRU mechanics (iterator to altlru_list item is holded in AltLRU_CacheEntry)
		- remove altlru_map item by iterator contained in altlru_list to cache out unused elements or with AltLRU::remove with
	*/
};

#endif
