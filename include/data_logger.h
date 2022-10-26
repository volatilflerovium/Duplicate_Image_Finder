/*********************************************************************
* Logger class                                                       *
* DataLogger class                               				         *
* Printer class                                  				         *
*                                                                    *
* Date:    17-05-2021                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <mutex>
#include <iostream>
#include <fstream>
#include <fstream>
#include <cstring>
#include <string>
#include <sys/stat.h>


//#include <filesystem>
//#define NDEBUG
#include <cassert>

#include "file_manager.h"

//####################################################################

template<typename IO, typename T>
static void Output2(IO& stream, T t){
	stream<<t;
}

//----------------------------------------------------------------------

template<typename IO, typename T, typename... Args>
static void Output2(IO& stream, T t, Args... args){
	Output2(stream, std::forward<T>(t));
	Output2(stream, std::forward<Args>(args)...);			
}

//----------------------------------------------------------------------

template<typename IO, typename T>
static void Output(IO& stream, T t){
#ifndef NDEBUG 
	stream<<t<<"\n";
#endif
}

//----------------------------------------------------------------------

template<typename IO, typename T, typename... Args>
static void Output(IO& stream, T t, Args... args){
#ifndef NDEBUG 
	Output2(stream, std::forward<T>(t), std::forward<Args>(args)...);
	stream<<"\n";
#endif
}

//####################################################################

class Logger
{
	public:
		template<typename... Args>
		static void log(Args... args){
			logMutex.lock();
			Output(std::cout, args...);
			logMutex.unlock();
		}

	private:
		static std::mutex logMutex;
};

//####################################################################

class DataLogger
{
	public:
		DataLogger(const char* filePath);
		virtual ~DataLogger();

		void flushBuffer();

		template<typename... Args>
		void logData(Args... args);

		void reset();

	private:	
		std::mutex m_logMutex;
		std::ofstream m_logFile;

		DataLogger()=delete;
		DataLogger(const DataLogger&)=delete;
};

//----------------------------------------------------------------------

template<typename... Args>
void DataLogger::logData(Args... args){
	m_logMutex.lock();
	Output(m_logFile, args...);
	m_logMutex.unlock();
}

//#####################################################################

class Printer
{	
	public:
		static void init(const char* filePath);
		static void init(const std::string& filePath);

		static void reset(){
			m_singleton->reset();
		}
		
		static void flushBuffer(){
			assert(m_singleton!=nullptr);
			m_singleton->flushBuffer();
		};

		template<typename... Args>
		static void logData(Args... args){
			assert(m_singleton!=nullptr);
			m_singleton->logData(std::forward<Args>(args)...);
		}

		~Printer()
		{}

	private:
		static DataLogger* m_singleton;

		Printer(const Printer&) = delete;
		Printer() = delete;
		Printer& operator=(const Printer&) = delete;
		
		class SingletonManager
		{
			private:
				DataLogger* m_sgtn;
		
			public:
				SingletonManager(DataLogger* sgtn_)
				:m_sgtn(sgtn_){};

				~SingletonManager(){
					if(m_sgtn!=nullptr){
						delete m_sgtn;
					}
				};
				SingletonManager()=delete;
				SingletonManager(const SingletonManager&)=delete;		
				SingletonManager& operator=(const SingletonManager&) = delete;
		};
};

//#####################################################################
//#####################################################################

#endif
