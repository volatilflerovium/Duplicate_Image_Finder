#include "data_logger.h"

std::mutex Logger::logMutex;


//#####################################################################

DataLogger::DataLogger(const char* filePath)
	:m_logFile(FileManager::setPath(filePath), std::ios::app)
{
	if(!m_logFile.is_open()){
		throw "Fail to open log file";
	}
}

DataLogger::~DataLogger() {
	m_logFile.close();
}

void DataLogger::flushBuffer() {
	m_logFile.flush();
}

void DataLogger::reset(){
	m_logFile.seekp(0);
}

//######################################################################

DataLogger* Printer::m_singleton=nullptr;

void Printer::init(const char* filePath) {	
	if(!m_singleton){
		m_singleton=new DataLogger(filePath);
		static SingletonManager SgM(m_singleton);
	}
}

//----------------------------------------------------------------------

void Printer::init(const std::string& filePath)
{
	init(filePath.c_str());
}

//#####################################################################
