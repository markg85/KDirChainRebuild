#pragma once
#ifndef ALTLRU_CACHEENTRY_HPP
#define ALTLRU_CACHEENTRY_HPP

#include <cstring>
#include <ctime>
#include "AltLRU_Types.hpp"

// This is class for holding data in cache structure
template <typename T>
class AltLRU_CacheEntry {
	typename AltLRUType<T>::altlru_list::iterator listiter;
	T value;
	size_t exp;

public:
	// Captain: constructor, init things
	AltLRU_CacheEntry(const T &v, const size_t expires=0) {
		init(v, expires);
	}
	virtual ~AltLRU_CacheEntry() {}

	// Its able to reassign values if 'set' called second time on same key
	void init(const T &v, const size_t expires=0)  {
		value = v; // Should we copy ?
		if( expires ) {
			time_t seconds = time(NULL);
			exp = expires + seconds;
		}else{
			exp = 0;
		}
	}

	inline T getvalue() const {
		return value;
	}
	inline T *getvalue_ref() {
		return &value;
	}
	// Check expire time is comed
	inline bool isExpired(time_t current_seconds=0) const {
		if( !exp ) {
			return false;
		}
		if( !current_seconds ) {
			current_seconds = time(NULL);
		}
		return ( static_cast<size_t>(current_seconds) > exp );
	}
	// Get the iterator to corresponding item in LRU 'list'
	inline typename AltLRUType<T>::altlru_list::iterator &getListIter() {
		return listiter;
	}
	// Set iterator
	inline void setListIter( typename AltLRUType<T>::altlru_list::iterator it) {
		listiter = it;
	}
};

#endif

