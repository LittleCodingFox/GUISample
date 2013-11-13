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
```
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
			"KeyboardInputEnabled": Optional, Whether the user may interact with this with a keyboard (true or false, not as a string),
			"Visible": Optional, Whether this is visible (true or false, not as a string),
			"Opacity": Optional, Opacity of this element (0 to 1, not as a string),
			"Tooltip": Optional, Tooltip text,
			
			"Children": [
				<Element here>
			]
		}
	]
]
```

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
	
Example JSON
------------
```JSON
[
	"Novel": [
		"DialogPanel": {
			"Control": "Frame",
			"Xpos": "20",
			"Ypos": "440",
			"Wide": "920",
			"Tall": "140",
		
			"Children": [
				"DialogText": {
					"Control": "Text",
					"Xpos": "0",
					"Ypos": "0",
					"Wide": "920",
					"Tall": "570",
					"FontSize": 24,
					"Text": "I'm so bored, what should we do?",
					"ExpandHeight": false
				},
				"DialogSkipSprite": {
					"Control": "Sprite",
					"Xpos": "900",
					"Ypos": "130",
					"Path": "continue.png",
					"CropTiled": "15, 20, 0, 0"
				}
			]
		},
		"NamePanel": {
			"Control": "Frame",
			"Xpos": "20",
			"Ypos": "380",
			"Wide": "120",
			"Tall": "20",
			
			"Children": [
				"NameText": {
					"Control": "Text",
					"Xpos": "center",
					"Ypos": "-4",
					"Wide": "100%",
					"Tall": "19",
					"FontSize": 19,
					"Text": "Your cousin"
				}
			]
		},
		"QuestionPanel": {
			"Control": "Frame",
			"Xpos": "center",
			"Ypos": "12%",
			"Wide": "360",
			"Tall": "270",

			"Children": [
				"HeaderText": {
					"Control": "Text",
					"Xpos": "center",
					"Ypos": "0",
					"Wide": "46",
					"Tall": "46",
					"FontSize": 46,
					"Text": "?",
					"ExpandHeight": false
				},
				"DialogQuestionAnswer1Text": {
					"Control": "Button",
					"Xpos": "center",
					"Ypos": "80",
					"Wide": "85%",
					"Tall": "10",
					"FontSize": 19,
					"Caption": "Eat cabbages",
					"ExpandHeight": false
				},
				"DialogQuestionAnswer2Text": {
					"Control": "Button",
					"Xpos": "center",
					"Ypos": "150",
					"Wide": "85%",
					"Tall": "10",
					"FontSize": 19,
					"Caption": "Find all the licorice",
					"ExpandHeight": false
				},
				"DialogQuestionAnswer3Text": {
					"Control": "Button",
					"Xpos": "center",
					"Ypos": "220",
					"Wide": "85%",
					"Tall": "10",
					"FontSize": 19,
					"Caption": "Drive a hoverboard",
					"ExpandHeight": false
				}
			]
		}
	]
]
```
