#ifndef CPP_UTIL_TIMER_HPP_
#define CPP_UTIL_TIMER_HPP_

/**
 * @file timer.hpp
 * The timer file that includes all of the implementations.  
 * Only one C++ source file should include this file, otherwise, there
 * some of the functions will be multiply defined.
 */
 
/*
 * David Gleich
 * 24 November 2006
 * Copyright, Stanford University
 */

#include <string>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <list>
#include <utility>

namespace util { 
namespace timer {
	using namespace boost::posix_time;
	
	namespace detail {
		
		/**
		 * Convert a time_duration constant into a double value counting  
		 * seconds.
		 * 
		 * @param d the time_duration object from boost::posix_time
		 * @return the time_duration length in seconds represented as a double
		 */
		double time_duration_to_double_seconds(time_duration d)
		{
			return (double(d.ticks())/double(time_duration::ticks_per_second()));
		}
	}
	
	/**
	 * The cpu_timer class measures elapsed CPU time.  CPU time is different
	 * than wall clock time, because wall clock time measures the time 
	 * from the start until the time until the end, not just the time
	 * spent running the process.
	 */  
	class cpu_timer
	{
	public:
		cpu_timer(void)
		: _state(TIMER_STOPPED),  _cpu_time(0.0)
		{}
					
		/**
		 * Restart the timer by setting the time to 0 and 
		 * starting the timer fresh.
		 */
		void restart() { _cpu_time = 0.0;  
						 _state = TIMER_STOPPED; start(); }
			
		/**
		 * Start the timer.  If the timer is running, this does
		 * not affect anything.
		 */			 
		void start() { update_cpu_time(); _state = TIMER_RUNNING; }
		
		/**
		 * Pause the timer.  If the timer is current paused, this does
		 * not affect anything. 
		 */
		void pause() { update_cpu_time(); _state = TIMER_STOPPED; }
		
		/**
		 * Get the elapsed time, the total time from the last time the timer was
		 * restarted and without all the time in the pauses.
		 * 
		 * @return the total elapsed time of the timer.
		 */   
		double elapsed() const {
			std::clock_t c = std::clock(); 
			return (_cpu_time + double(_state)*double(c - _last_clock)/CLOCKS_PER_SEC);
		}
			
	protected:
		/**
		 * Update CPU time updates the last time the cpu clock was checked and
		 * adds any time needed to the recorded time if the clock is running.
		 */
		void update_cpu_time() { 
			std::clock_t c = std::clock();
			_cpu_time += double(_state)*double(c - _last_clock)/CLOCKS_PER_SEC; 
			_last_clock = c;
		}
		
		std::clock_t _last_clock;
		double _cpu_time;
		enum { TIMER_RUNNING=1, TIMER_STOPPED=0 } _state;
	};
	
	class wall_timer
	{
	
	public:
		wall_timer(void)
		: _state(TIMER_STOPPED), _wall_time(0,0,0,0)
		{}
		
		void restart() { _wall_time = time_duration(0,0,0,0); 
						 _state = TIMER_STOPPED; start(); }
		void start() { update_wall_time(); _state = TIMER_RUNNING; }
		void pause() { update_wall_time(); _state = TIMER_STOPPED; }
		double elapsed() const { 
			time_duration d = microsec_clock::local_time() - _last_time;
			return (detail::time_duration_to_double_seconds(_wall_time) 
						+ double(_state)*detail::time_duration_to_double_seconds(d));
		}
		
	protected:
		/**
		 * Update wall time updates the last time the wall clock was checked and
		 * adds any time needed to the recorded time if the clock is running.
		 */
		void update_wall_time() { 
			ptime t = microsec_clock::local_time();
			time_duration d = t - _last_time;
			if (_state == TIMER_RUNNING) {
				_wall_time += d;
			} 
			_last_time = t;
		}
	
		ptime _last_time;
		time_duration _wall_time;
		enum { TIMER_RUNNING=1, TIMER_STOPPED=0 } _state;
	};
	
	/**
	 * The cpu_wall_timer class records both the CPU time and the wall time
	 * for a timer object.
	 */
	class cpu_wall_timer
	: public cpu_timer, public wall_timer
	{
	public:
		cpu_wall_timer(void)
		: cpu_timer(), wall_timer()
		{}			
		
		void restart() { cpu_timer::restart(); wall_timer::restart(); }
						 
		void start() { cpu_timer::start(); wall_timer::start(); }
		void pause() { cpu_timer::pause(); wall_timer::pause(); }
		double elapsed_wall() const { 
			return (wall_timer::elapsed());
		}
		double elapsed_cpu() const { 
			return (cpu_timer::elapsed());
		}
		
		std::pair<double,double> elapsed() const {
			return std::make_pair(elapsed_wall(), elapsed_cpu());
		}
			
	};
		
	/**
	 * The default timer_pointer is a cpu_wall_timer.
	 */		
	typedef cpu_wall_timer* timer_pointer;
	
	namespace detail {
		
		class multi_timer_stage
		{
		private:
			std::list<multi_timer_stage *> _substages;
			typedef std::list<std::pair<std::string, timer_pointer> > timer_list;
			timer_list _timers;
			
		public:
			double _time;
			cpu_wall_timer _timer;
			std::string _name;		
			
			multi_timer_stage(const char* name)
			: _name(name), _time(0), _timer() 
			{
				_timer.start();
			}
			
			multi_timer_stage(const std::string& name)
			: _name(name), _time(0), _timer() 
			{
				_timer.start();
			}
			
			multi_timer_stage* push(const std::string& name)
			{
				multi_timer_stage* next = new multi_timer_stage(name);
				_substages.push_back(next);
				
				return (next);	
			}
			
			void pop()
			{
				// stop my timer
				_timer.pause();
				_time = _timer.elapsed_wall();
				
				// stop all my subtimers
				for (timer_list::iterator i = _timers.begin(); 
					 i != _timers.end(); ++i)
				{
					//(*i).second->pause();
					i->second->pause();
				}
			}
			
			timer_pointer get_timer(const std::string& name)
			{
				timer_pointer rval = new cpu_wall_timer();
				_timers.push_back(std::make_pair(std::string(name),rval));
				return (rval);
			}
			
			void recursive_dump(int level)
			{
				_time = _timer.elapsed_wall();
				
				std::string indent = std::string(2*(level), ' ');
				std::cout << indent << _name << " -- " << _time << std::endl;
				for (timer_list::iterator i = _timers.begin(); 
					 i != _timers.end(); ++i)
				{
					std::cout << indent << "[timer] " << (*i).first << " -- " << i->second->elapsed_wall() << std::endl; 
				}	
				for (std::list<multi_timer_stage*>::iterator i = _substages.begin(); 
					 i != _substages.end(); ++i)
				{
					(*i)->recursive_dump(level+1); 
				}	
			}
		};
	}
	
	class multi_timer
	{
	private:
		typedef detail::multi_timer_stage timer_stage;
		timer_stage _root;
		
		std::list<timer_stage *> _stack;
		std::map<std::string, timer_pointer> _timer_map;
		
		bool _auto_dump;
		
		
	public:
		
		multi_timer(bool auto_dump=false)
		: _root("total"), _auto_dump(auto_dump)
		{
			check_stack();
		}
		
		~multi_timer()
		{
			if (_auto_dump) { dump(); }
			
			// free all the timers
			
		}
		
		
		void push(const char *name)
		{
			std::string stls(name);
			push(stls);
		}
		
		void push(const std::string& name)
		{
			timer_stage *_next = _stack.front()->push(name);
			_stack.push_front(_next);
		}
		
		void pop()
		{
			if (_stack.size() > 0)
			{
				_stack.front()->pop();
				_stack.pop_front();
			}			
		}
		
		timer_pointer add(const char *name)
		{
			std::string stls(name);
			return (add(stls));
		}
		timer_pointer add(const std::string& name)
		{
			return (_stack.front()->get_timer(name));
		}
		
		timer_pointer get(const char *name)
		{
			std::string stls(name);
			return (get(stls));
		}
		timer_pointer get(const std::string& name)
		{
			if (_timer_map.count(name) == 0)
			{
				timer_pointer rval = new cpu_wall_timer();
				_timer_map[name] = rval;
			}
			
			return _timer_map[name]; 
		}
		
		void dump()
		{
			
			_root.recursive_dump(0);
		}
		
		/**
		 * Set the auto_dump property of the multi_timer
		 */
		void auto_dump(bool auto_dump=true)
		{
			_auto_dump = auto_dump;
		}
		
	private:
		/**
		 * The check stack function checks the stack to make sure it is valid
		 * before a call that uses the stack.
		 * 
		 * The stack is valid if there is at least one thing on it.  If
		 * there is nothing on the stack, then check_stack will push
		 * the root node again.
		 */
		void check_stack(void)
		{
			if (_stack.size() == 0)
			{
				_stack.push_front(&_root);
			}
		}
	};
	
	namespace global {
		
		/*
		 * This section lists all the prototypes for the global
		 * functions.  The global functions are used to manage
		 * a global multi_timer to avoid passing a single
		 * multi_timer class around throughout all the functions
		 * in a program.
		 */

		void push(const char *name);
		void push(const std::string &name);
		void pop();
		void dump();
		
		timer_pointer add_to_stage(const char *name);
		timer_pointer add_to_stage(const std::string &name);
		timer_pointer get(const char *name);
		timer_pointer get(const std::string &name);
		
	} // namespace global
		
	
} // namespace timer
} // namespace util


#endif /*CPP_UTIL_TIMER_HPP_*/
