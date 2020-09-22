// Original version of this file provided by Lonewolf

#ifndef COGRE_TEXT_CPP
#define COGRE_TEXT_CPP

#include <Ogre.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreStringConverter.h>

using namespace Ogre;

// Class displays text in Ogre using an Overlay
class COgreText
{
public:
	// Constructor
	COgreText();

	// Destructor
	~COgreText();

	// Set all of the Text properties
	void SetAll(String _sText, float _fX = 0.5, float _fY = 0.5, float _fTextSize = 0.1, ColourValue _cColour = ColourValue(0.5, 0.5, 0.5), String _sFontName = "DansMac");

	// Set the Text to display in the TextArea
	void SetText(String _sText);

	// Set the Position the TextArea should be displayed at
	// NOTE: fX and fY should be between 0.0 and 1.0
	void SetPosition(float _fX = 0.5, float _fY = 0.5);

	// Set the Size of the Text
	void SetTextSize(float _fTextSize = 0.2);

	// Set the Colour of the Text
	// The SetColour with 2 Colours forms a gradient from TopColour to BottomColour on the Text
	// NOTE: Which even SetColour function is called last is the Colour used
	void SetColour(ColourValue _cColour = ColourValue(0.5, 0.5, 0.5));
	void SetColour(ColourValue _cTextTopColour, ColourValue _cTextBottomColour);

	// Get the Colour of the Text
	ColourValue GetColour() const;

	// Set the Font to use
	void SetFont(String _sFontName = "DansMac");

	// Set whether to show the TextArea or not
	bool Show(bool _bShowText);

private:
	OverlayManager* mcpOverlayManager;			// Holds a pointer to the Overlay Manager
	OverlayContainer* mcpPanel;					// Holds a pointer to the Overlay Container
	Overlay* mcpOverlay;						// Holds a pointer to the Overlay
	TextAreaOverlayElement* mcpTextArea;		// Holds a pointer to the current TextArea
	
	static int SmiNumberOfTextAreaInstances;	// Holds the total number of TextAreas created
};

#endif
