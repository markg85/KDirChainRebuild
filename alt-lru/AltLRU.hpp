#pragma once
#ifndef ALTLRU_HPP
#define ALTLRU_HPP

#include <functional>
#include <string>
#include <cstring>
#include <ctime>

#include "AltLRU_Types.hpp"
#include "AltLRU_CacheEntry.hpp"

template <typename T>
class AltLRU {
private:
	size_t max_count; // Maximum cached items count; 0=unlim
	size_t stored_items; // Current count of stored items for stat and max_count checking with O(1)
	size_t expire; // Maximum live time for items(in seconds); 0=unlim


	typename AltLRUType<T>::altlru_map valuemap;
	typename AltLRUType<T>::altlru_list valuelist;

	inline void cacheRemove(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id) {
		auto it = valuemap.find( make_pair(keyname, id) );
		if( it != valuemap.end() ) {
			cacheRemove(it);
		}
	}
	void cacheRemove(const typename  AltLRUType<T>::altlru_map_iter &it);

	inline void cacheOut(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id) {
		auto it = valuemap.find( make_pair(keyname, id) );
		if( it != valuemap.end() ) {
			cacheOut(it);
		}
	}
	void cacheOut(const typename  AltLRUType<T>::altlru_map_iter &it);

public:
#ifdef ALTLRU_HITINFO
	size_t stat_cache_hits, stat_cache_miss, stat_cache_get, stat_cache_set, stat_cache_out;
#endif
	AltLRU(size_t max_count, size_t expire=0);
	virtual ~AltLRU();
	void clear();
	void checkLimits();
	void checkExpires();

	// returns count items stored in LRU
	inline size_t count() const {
		return stored_items;
	}

	// Get cache key. 'id' is used to add uniqueness to keyname(if needed) e.g. AltLRU::get("user", 1234);
	inline const T *get(const std::string &keyname) {
		return get(keyname, 0);
	}
	const T *get(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id);

	// Set cache key. Default expire is never
	inline void set(const std::string &keyname, const T &value, size_t expire=0) {
		set(keyname, 0, value, expire);
	}
	void set(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id, const T &value, size_t expire=0);

	// Same as 'get' for 'remove'
	inline void remove(const std::string &keyname) {
		remove(keyname, 0);
	}
	void remove(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id);

	// Get value or set it if its not found. Accepts lambda syntax so it easy to use
	inline const T *getset(const std::string &keyname, std::function< T() > getkey_callback, size_t expire=0) {
		return getset(keyname, 0, getkey_callback, expire);
	}
	const T *getset(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id, std::function< T() > getkey_callback, size_t expire=0);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Captain: this is constructor, init thigs */
template <typename T>
AltLRU<T>::AltLRU(size_t max_count, size_t expire) {
	this->max_count = max_count;
	this->expire = expire;
	clear();
	valuemap.max_load_factor( 3 ); // Set max_load per bucket to 3
	valuemap.rehash( max_count * 1.1 ); // Cheate count of buckets to hold max_count values
}

/* We should delete what we created with new AltLRU_CacheEntry */
template <typename T>
AltLRU<T>::~AltLRU() {
	clear();
}

/* Clear cache contents and free allocated mem; O(N) */
template <typename T>
void AltLRU<T>::clear() {
	// Clear the cache contents
	for( auto &i : valuelist ) {
		delete (*i).second;
	}
	valuelist.clear();
	valuemap.clear();
	stored_items=0;

#ifdef ALTLRU_HITINFO
	stat_cache_hits=stat_cache_miss=stat_cache_get=stat_cache_set=stat_cache_out=0;
#endif
}

// Check current limits and remove most unused elements to fit limits
template <typename T>
void AltLRU<T>::checkLimits() {
	while( max_count>0 && stored_items>max_count ) {
		// Max count/size excess, remove most unused entry
		auto i = valuelist.end();
		i--;
		cacheOut( *i ); // O(1)
	}
}

// Remove all expired elements
template <typename T>
void AltLRU<T>::checkExpires() {
	time_t current_seconds = time(NULL);
	for( auto i=valuemap.begin(); i<valuemap.end();) {
		if( i.second->isExpired(current_seconds) ) {
			auto del_it = i;
			i++;
			cacheOut( del_it );
		}else{
			i++;
		}
	}
}

/*
	Get cache key by name + id. 'id' is used to add uniqueness to keyname(if needed) e.g. AltLRU::get("user", 1234);
*/
template <typename T>
const T *AltLRU<T>::get(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id) {
#ifdef ALTLRU_HITINFO
	stat_cache_get++;
#endif
	auto it = valuemap.find( make_pair(keyname, id) ); // O(1)
	if( it == valuemap.end() ) {
		// Not found
#ifdef ALTLRU_HITINFO
		stat_cache_miss++;
#endif
		return nullptr;
	}else{
		if( (*it).second->isExpired() ) {
			// Cache entry is expired so cacheout it
			cacheOut(it); // O(1)
#ifdef ALTLRU_HITINFO
			stat_cache_miss++;
#endif
			return nullptr;
		}

		// Got record, we must move it to 1st place in valuelist; O(1)
		valuelist.splice(valuelist.begin(), valuelist, (*it).second->getListIter());
		//Set cacheentry's listiter to new iterator
		(*it).second->setListIter( valuelist.begin() );
#ifdef ALTLRU_HITINFO
		stat_cache_hits++;
#endif
		return (*it).second->getvalue_ref();
	}
}

// Set cache key. Default expire is never. Same semantics for 'id' as in 'get'
template <typename T>
void AltLRU<T>::set(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id, const T &value, size_t expire) {
#ifdef ALTLRU_HITINFO
	stat_cache_set++;
#endif
	auto ipair = make_pair(keyname, id);
	
	if( !expire ) {
		expire = this->expire;
	}

	// Insert entry into valuemap
	auto mapinsert = valuemap.insert( make_pair( ipair, nullptr) ); // O(1)
	auto it = mapinsert.first;

	if( mapinsert.second ) {
		// A new value was inserted
		(*it).second = new AltLRU_CacheEntry<T>(value, expire);
		// Insert iterator to map intry into valuelist
		valuelist.push_front( it ); // O(1)
		(*it).second->setListIter( valuelist.begin() );
		stored_items++;
		checkLimits();
	}else{
		// Found, change values
		(*it).second->init(value, expire); // Set new values
		valuelist.splice(valuelist.begin(), valuelist, (*it).second->getListIter()); //Move iterator to front of list
		//Set cacheentry's listiter to new iterator
		(*it).second->setListIter( valuelist.begin() );
	}
}

// Same as 'get' for 'remove'
template <typename T>
void AltLRU<T>::remove(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id) {
	cacheOut(keyname, id);
}

/* Get value or set it if its not found. Accepts lambda syntax so it easy to use:
	int user_id = 1234;
	int friends_count;
	friends_count = lru.getset("user_friends_count", user_id, [user_id]() -> decltype(friends_count) {
		return get_user_count(user_id);
	});

	This will get "user_friends_count"+user_id key from cache or call lambda to obtain value (and set that value to cache)
	Surely, lambda wil not be called if "user_friends_count"+user_id key exists.
*/
template <typename T>
const T *AltLRU<T>::getset(const std::string &keyname, const typename AltLRUType<T>::altlru_key_add_id &id, std::function< T() > getkey_callback, size_t expire) {
	auto valr = get(keyname, id);
	if( valr == nullptr ) {
		auto val = getkey_callback();
		set(keyname, id, val, expire);
		auto v = get(keyname, id);
		return v;
	}else{
		return valr;
	}
}

/* cache remove item. For internal use, differs from cacheOut (wont run cacheout routines) */
template <typename T>
void AltLRU<T>::cacheRemove(const typename  AltLRUType<T>::altlru_map_iter &it) {
	auto cacheEntry = (*it).second;
	valuelist.erase( cacheEntry->getListIter() );
	valuemap.erase( it );
	delete cacheEntry;
}

/* cacheout item. for internal use */
template <typename T>
void AltLRU<T>::cacheOut(const typename  AltLRUType<T>::altlru_map_iter &it) {
	// TODO Call a cacheout callback
	cacheRemove(it);
	stored_items--;
#ifdef ALTLRU_HITINFO
	stat_cache_out++;
#endif
}

#endif
