/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* Version: 1.0                                                       *
* Date:    29-05-2021   (Reviewed 03/2025)                           *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef WX_CONSTANTS_H
#define WX_CONSTANTS_H

//====================================================================

namespace WX{
	enum 
	{
		BROWSE=101,
		PICTURE_SCROLLER,
		PICTURE_VIWER,
		ADD_PIC,
		CLEAR,
		START,
		START_FROM_POPUP,
		DELETE_IMG,
		OPEN_IMG,
		SENSITIVITY_MODE,
		RESET,
		REMOVE_DIR,
	};

	const int MIN_WIDTH=800;
	const int MIN_HEIGHT=800;
};

const int c_windowPadding=10;
const int c_picPadding=5;
const int c_scrollerWidth=130;
const int c_scrollerMinHeight=300;

#endif
