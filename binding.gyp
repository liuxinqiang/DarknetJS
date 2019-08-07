{
    "targets": [
        {
            "target_name": "darknet",
            "sources": [
                "cplusplus/entry.cpp",
                "cplusplus/detect/improcess.cpp",
                "cplusplus/detect/detect.cpp"
            ],
			"conditions": [
				['OS=="win"', {
					"include_dirs": [
						"./lib/opencv/include",
						"./lib/darknet/include"
					],
					"link_settings": {
						 "libraries": [
							"../lib/opencv/lib/opencv_world345",
							"../lib/darknet/lib/yolo_cpp_dll"
						 ],
					}
				}],
				['OS=="mac"', {
				}],
				['OS=="linux"', {
				}],
			]
        }
    ]
}