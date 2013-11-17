[
	"Main": [
		"Window": {
			"Control": "Window",
			"Wide": "45%",
			"Tall": "20%",
			"Xpos": "center",
			"Ypos": "center",
			
			"Children": [
				"ResolutionText": {
					"Control": "Text",
					"Wide": "100%",
					"Tall": "20",
					"FontSize": 20,
					"ExpandHeight": true,
					"Text": "Resolution:"
				},
				"ResolutionChoice": {
					"Control": "Dropdown",
					"Wide": "200",
					"Xpos": "120",
					"Elements": "800x600|1024x768|1280x1024|1920x1080"
				},
				"QualityText": {
					"Control": "Text",
					"Wide": "100%",
					"Tall": "20",
					"FontSize": 20,
					"ExpandHeight": true,
					"Text": "Quality:",
					"Ypos": "30"
				},
				"QualityChoice": {
					"Control": "Dropdown",
					"Wide": "200",
					"Xpos": "120",
					"Ypos": "30",
					"Elements": "Low|Medium|High|Very High"
				},
				"SSAOSwitch": {
					"Control": "Checkbox",
					"Checked": true,
					"Caption": "SSAO",
					"Xpos": "10",
					"Ypos": "70"
				},
				"HDRSwitch": {
					"Control": "Checkbox",
					"Checked": true,
					"Caption": "HDR",
					"Xpos": "80",
					"Ypos": "70"
				},
				"AcceptButton": {
					"Control": "Button",
					"Caption": "Accept",
					"Xpos": "30%",
					"Ypos": "110"
				},
				"CancelButtonButton": {
					"Control": "Button",
					"Caption": "Cancel",
					"Xpos": "60%",
					"Ypos": "110"
				}
			]
		}
	]
]
