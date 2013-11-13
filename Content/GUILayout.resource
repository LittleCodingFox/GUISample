[
	"Novel" : [
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
		"NamePanel" : {
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
		"QuestionPanel" : {
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
				"DialogQuestionAnswer1Text" : {
					"Control": "Button",
					"Xpos": "center",
					"Ypos": "80",
					"Wide": "85%",
					"Tall": "10",
					"FontSize": 19,
					"Caption": "Eat cabbages",
					"ExpandHeight": false
				},
				"DialogQuestionAnswer2Text" : {
					"Control": "Button",
					"Xpos": "center",
					"Ypos": "150",
					"Wide": "85%",
					"Tall": "10",
					"FontSize": 19,
					"Caption": "Find all the licorice",
					"ExpandHeight": false
				},
				"DialogQuestionAnswer3Text" : {
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
