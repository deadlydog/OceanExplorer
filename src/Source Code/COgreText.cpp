#include "COgreText.h"

// Initialize the Number Of TextAreas created
int COgreText::SmiNumberOfTextAreaInstances = 0;


// Constructor
COgreText::COgreText()
{
	// Get a handle to the Overlay Manager singleton
	mcpOverlayManager = OverlayManager::getSingletonPtr();

	// If this is the very first time an COgreText object has been created
	if (SmiNumberOfTextAreaInstances == 0)
	{
		// Create the Overlay and Panel to which all TextAreas will be rendered on

		// Create the Panel object
		mcpPanel = static_cast<OverlayContainer*>(mcpOverlayManager->createOverlayElement("Panel","GUI"));
		mcpPanel->setMetricsMode(Ogre::GMM_PIXELS);
		
		// Set Panel to cover the entire screen
		mcpPanel->setPosition(0, 0);
		mcpPanel->setDimensions(1.0f, 1.0f);

		// Create the Overlay and attach the Panel to it
		mcpOverlay = mcpOverlayManager->create("GUIOverlay");
		mcpOverlay->add2D(mcpPanel);
	}

	// Increment the number of TextArea instances and create a unique string ID for this one
	this->SmiNumberOfTextAreaInstances++;
	String sInstanceID = "TextAreaElement" + StringConverter::toString(SmiNumberOfTextAreaInstances);

	// Get a handle to the Overlay Manager and Panel
	mcpOverlay = mcpOverlayManager->getByName("GUIOverlay");
	mcpPanel = static_cast<OverlayContainer*>(mcpOverlayManager->getOverlayElement("GUI"));

	// Create the TextArea and attach it to the Panel
	mcpTextArea = static_cast<TextAreaOverlayElement*>(mcpOverlayManager->createOverlayElement("TextArea", sInstanceID));
	mcpPanel->addChild(mcpTextArea);

	// Make sure the TextAreas are being shown
	mcpOverlay->show();
}

// Destructor
COgreText::~COgreText()
{
	// Create the string ID of the TextArea to destroy and destroy it
	String sInstanceID = "TextAreaElement" + StringConverter::toString(SmiNumberOfTextAreaInstances);
	mcpOverlayManager->destroyOverlayElement(sInstanceID);

	mcpTextArea = NULL;

	// Decrement the number of remaining TextAreas
	this->SmiNumberOfTextAreaInstances--;

	// If this is the last COgreText instance to be destroyed
	if (SmiNumberOfTextAreaInstances == 0)
	{
		// Also destroy the Panel and Overlay
		mcpOverlayManager->destroyOverlayElement("GUI");
		mcpOverlayManager->destroy("GUIOverlay");
	}
}

// Set all of the Text properties
void COgreText::SetAll(String _sText, float _fX, float _fY, float _fTextSize, ColourValue _cColour, String _sFontName)
{
	SetText(_sText);
	SetPosition(_fX, _fY);
	SetTextSize(_fTextSize);
	SetColour(_cColour);
	SetFont(_sFontName);
}

// Set the Text to display in the TextArea
void COgreText::SetText(String _sText)
{
	// If we have a handle to the TextArea
	if (mcpTextArea != NULL)
	{
		mcpTextArea->setCaption(_sText);
		mcpTextArea->setDimensions(1.0f, 1.0f);
		mcpTextArea->setMetricsMode(Ogre::GMM_RELATIVE);
	}
}

// Set the Position the TextArea should be displayed at
// NOTE: fX and fY should be between 0.0 and 1.0
void COgreText::SetPosition(float _fX, float _fY)
{
	// If we have a handle to the TextArea
	if (mcpTextArea != NULL)
	{
		// Set the TextAreas Position
		mcpTextArea->setPosition(_fX, _fY);
	}
}

// Set the Size of the Text
void COgreText::SetTextSize(float _fTextSize)
{
	// If we have a handle to the TextArea
	if (mcpTextArea != NULL)
	{
		mcpTextArea->setCharHeight(_fTextSize);
	}
}

// Set the Colour of the Text
void COgreText::SetColour(ColourValue _cColour)
{
	// If we have a handle to the TextArea
	if (mcpTextArea != NULL)
	{
		// Set the Colour to use for the Text
		mcpTextArea->setColour(_cColour);
	}
}

// Set the Colour of the Text
void COgreText::SetColour(ColourValue _cTextTopColour, ColourValue _cTextBottomColour)
{
	// If we have a handle to the TextArea
	if (mcpTextArea != NULL)
	{
		mcpTextArea->setColourTop(_cTextTopColour);
		mcpTextArea->setColourBottom(_cTextBottomColour);
	}
}

// Get the Colour of the Text
ColourValue COgreText::GetColour() const
{
	ColourValue sColor;

	// If we have a handle to the TextArea
	if (mcpTextArea != NULL)
	{
		sColor = mcpTextArea->getColour();
	}

	return sColor;
}

// Set the Font to use
void COgreText::SetFont(String _sFontName)
{
	// If we have a handle to the TextArea
	if (mcpTextArea != NULL)
	{
		mcpTextArea->setFontName(_sFontName);
	}
}

// Set whether to show the TextArea or not
bool COgreText::Show(bool _bShowText)
{
	// If we have a handle to the TextArea
	if (mcpTextArea != NULL)
	{
		// If the TextArea should be shown
		if (_bShowText)
		{
			// Show the TextArea
			mcpTextArea->show();
		}
		// Else the TextArea should be hidden
		else
		{
			// Hide the TextArea
			mcpTextArea->hide();
		}
	}
	// Else we can't display the Text
	else
	{
		return false;
	}

	// Return if the Text is being shown or not
	return _bShowText;
}