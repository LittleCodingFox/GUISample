Introduction
------------
This is a custom OpenGL GUI using SFML for input and window handling. Both input and window handling can be easily replaced.

Status
------
This library is relatively stable and is usable, but needs several fixes to the behaviour of widgets. You can still use it for something pretty complex if you don't mind things not working as well as other GUIs.

License
-------
Source licensed under the zlib/libpng license (Excluding libraries used)

Credits
-------
Initial GUI implementation and code: Nuno Silva (Little Coding Fox/Anthe)
Inspiration, some base GUI logic, and PolyCode Theme: PolyCode team (http://www.polycode.org)

JSON Specs Reference
--------------------
[
	"LayoutName": [
		"ElementName": {
			"Control": "ControlName", (Check Available Controls list)
			"Xpos": "Optional, X position as string (may be Percentage, "center", or specific number)",
			"Ypos": "Optional, Y position as string (may be Percentage, "center", or specific number)",
			"Wide": "Width of the element (May be Percentage or specific number)",
			"Tall": "Height of the element (May be Percentage or specific number)",
			"Enabled": Optional, Whether the user may interact with this (true or false, not as a string),
			"MouseInputEnabled": Optional, Whether the user may interact with this with a mouse (true or false, not as a string),
			"KeyboardInputEnabled: Optional, Whether the user may interact with this with a keyboard (true or false, not as a string),
			"Visible": Optional, Whether this is visible (true or false, not as a string),
			"Opacity": Optional, Opacity of this element (0 to 1, not as a string),
			"Tooltip": Optional, Tooltip text,
			
			"Children": [
				<Element here>
			]
		}
	]
]

Available Controls and Properties
---------------------------------
	- Window
		- Closed (True/false)
	- Frame
	- Button
		- Caption (String)
		- FontSize (Number)
	- Checkbox
		- Checked (True/False)
	- Sprite
		- Path (String)
		- CropTiled (String in the format of "Width, Height, FrameIDX, FrameIDY" for evenly splitting an image)
	- Text
		- ExpandHeight (True/False, whether the height will be resized if too small for all text)
		- FontSize (Number)
		- Text (String)
		- Alignment (String, combination of "Left", "Right", "Center", "VCenter" separated by |'s, e.g. "Center|VCenter")
	- Group
	- Textbox
		- Text (String)
		- FontSize (Number)
	- List (Requires ScrollableFrame parent)
		- Elements (String, names separated by |'s, e.g. "Milk|Cookies")
		- FontSize (Number)
	- ScrollableFrame
	- HScroll
	- VScroll
	- Dropdown
		- Elements (String, names separated by |'s, e.g. "Milk|Cookies")
