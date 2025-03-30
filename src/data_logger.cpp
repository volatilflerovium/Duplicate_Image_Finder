/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*  
* Logger class                                                       *
* DataLogger class                               				         *
* Printer class                                  				         *
*                                                                    *
* Date:    17-05-2021  (Reviewed 03/2025)                            *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
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

DataLogger::~DataLogger()
{
	m_logFile.close();
}

void DataLogger::flushBuffer()
{
	m_logFile.flush();
}

void DataLogger::reset()
{
	m_logFile.seekp(0);
}

//######################################################################

DataLogger* Printer::m_singleton=nullptr;

void Printer::init(const char* filePath)
{	
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
