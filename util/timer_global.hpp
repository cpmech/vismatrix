#ifndef CPP_UTIL_GLOBAL_TIMER_HPP_
#define CPP_UTIL_GLOBAL_TIMER_HPP_

#include <util/timer.hpp>

/**
 * @file timer_global.hpp
 * This file includes all the definitions for the global routines 
 * and should only be included into your program a single
 * time to avoid linking errors with duplicate functions
 */

namespace util { 
namespace timer {
	namespace global {
		
		#ifndef CPP_UTIL_TIMER_GLOBAL_AUTODUMP
		#define CPP_UTIL_TIMER_GLOBAL_AUTODUMP true
		#endif /* CPP_UTIL_TIMER_GLOBAL_AUTODUMP */
		
		multi_timer t(CPP_UTIL_TIMER_GLOBAL_AUTODUMP);
		
		void push(const char *name) { t.push(name); }
		void push(const std::string &name) { t.push(name); }
		void pop() { t.pop(); }
		void dump() { t.dump(); }
		void auto_dump(bool flag) { t.auto_dump(flag); }
		
		timer_pointer add_to_stage(const char *name)
		{ return t.add(name); }
			
		timer_pointer add_to_stage(const std::string &name)
		{ return t.add(name); }
			
		timer_pointer get(const char *name)
		{ return t.get(name); }
		
		timer_pointer get(const std::string &name)
		{ return t.get(name); }
		
	} // namespace global
		
	
} // namespace timer
} // namespace util


#endif /*CPP_UTIL_GLOBAL_TIMER_HPP_*/
