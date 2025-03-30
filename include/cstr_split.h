/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* template<int N> class CstrSplit                                    *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _CSTR_SPLIT_H
#define _CSTR_SPLIT_H

#include <cstring>
//====================================================================

template<int N>
class CstrSplit
{
	public:
		CstrSplit(const char* data, const char* separator)
		: length(0)
		{
			 splitData(data, separator);
		}

		~CstrSplit()
		{
			for(int i=0; i<length; i++){
				delete[] chunks[i];
			}
		}

		char* operator[](int i)
		{
			if(i<length){
				return chunks[i];
			}
			else{
				throw "ERROR: index out of range.";
			}
		}

		int chunkSize(int p) const
		{
			return m_chunkSize[p];
		}

		int dataSize() const
		{
			return length;
		}

		//for debuging purposes	
		void print()
		{
			for(int i=0; i<length; i++){
				dbg(m_chunkSize[i], " :", chunks[i]);
			}
		}

	private:
		char* chunks[N];
		int m_chunkSize[N]; 
		int length;

		void splitData(const char* data, const char* separator);

};

//--------------------------------------------------------------------

template<int N>
void CstrSplit<N>::splitData(const char* data, const char* separator)
{
	int ck[N];
	int i=0;
	const int spl=std::strlen(separator);

	while(data[i]!='\0'){
		if(memcmp(data+i, separator, spl)==0){
			if(length<N){
				ck[length++]=i;
			}
		}
		i++;
	}

	if(length<N){
		ck[length++]=i;
	}

	int t=0;
	for(int j=0; j<length; j++){
		m_chunkSize[j]=ck[j]-t;
		chunks[j]=new char[ck[j]-t+1];
		memcpy(chunks[j], data+t, ck[j]-t);
		chunks[j][ck[j]-t]='\0';
		t=ck[j]+spl;
	}
}

//====================================================================

#endif
