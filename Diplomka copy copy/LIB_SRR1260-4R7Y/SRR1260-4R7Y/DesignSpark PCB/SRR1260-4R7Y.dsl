SamacSys ECAD Model
259708/291251/2.50/2/4/Inductor

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r540_290"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 2.900) (shapeHeight 5.400))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Default"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 50 mils)
			(strokeWidth 5 mils)
		)
	)
	(patternDef "SRR12604R7Y" (originalName "SRR12604R7Y")
		(multiLayer
			(pad (padNum 1) (padStyleRef r540_290) (pt -4.850, 0.000) (rotation 0))
			(pad (padNum 2) (padStyleRef r540_290) (pt 4.850, 0.000) (rotation 0))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0.000, 0.000) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 28)
			(line (pt -6.25 6.25) (pt 6.25 6.25) (width 0.2))
		)
		(layerContents (layerNumRef 28)
			(line (pt 6.25 6.25) (pt 6.25 -6.25) (width 0.2))
		)
		(layerContents (layerNumRef 28)
			(line (pt 6.25 -6.25) (pt -6.25 -6.25) (width 0.2))
		)
		(layerContents (layerNumRef 28)
			(line (pt -6.25 -6.25) (pt -6.25 6.25) (width 0.2))
		)
		(layerContents (layerNumRef 30)
			(line (pt -7.3 7.25) (pt 7.3 7.25) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 7.3 7.25) (pt 7.3 -7.25) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 7.3 -7.25) (pt -7.3 -7.25) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt -7.3 -7.25) (pt -7.3 7.25) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -6.25 -3.2) (pt -6.25 -6.25) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -6.25 -6.25) (pt 6.25 -6.25) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 6.25 -6.25) (pt 6.25 -3.2) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -6.25 3.2) (pt -6.25 6.25) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -6.25 6.25) (pt 6.25 6.25) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 6.25 6.25) (pt 6.25 3.2) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 6.25 3.2) (pt 6.25 3.2) (width 0.1))
		)
	)
	(symbolDef "SRR1260-4R7Y" (originalName "SRR1260-4R7Y")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 0 mils -35 mils) (rotation 0]) (justify "UpperLeft") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 800 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 800 mils -35 mils) (rotation 0]) (justify "UpperRight") (textStyleRef "Default"))
		))
		(arc (pt 250 mils -2 mils) (radius 50 mils) (startAngle 177.7) (sweepAngle -175.4) (width 6 mils))
		(arc (pt 350 mils -2 mils) (radius 50 mils) (startAngle 177.7) (sweepAngle -175.4) (width 6 mils))
		(arc (pt 450 mils -2 mils) (radius 50 mils) (startAngle 177.7) (sweepAngle -175.4) (width 6 mils))
		(arc (pt 550 mils -2 mils) (radius 50 mils) (startAngle 177.7) (sweepAngle -175.4) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 650 mils 250 mils) (justify Left) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "SRR1260-4R7Y" (originalName "SRR1260-4R7Y") (compHeader (numPins 2) (numParts 1) (refDesPrefix L)
		)
		(compPin "1" (pinName "1") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "2") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "SRR1260-4R7Y"))
		(attachedPattern (patternNum 1) (patternName "SRR12604R7Y")
			(numPads 2)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
			)
		)
		(attr "Mouser Part Number" "652-SRR1260-4R7Y")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Bourns/SRR1260-4R7Y?qs=meVWcSOgEr6HGQUyq2JiMQ%3D%3D")
		(attr "Manufacturer_Name" "Bourns")
		(attr "Manufacturer_Part_Number" "SRR1260-4R7Y")
		(attr "Description" "Inductor Power Shielded Wirewound 4.7uH 30% 100KHz 19Q-Factor Ferrite 6.8A 15.5mOhm DCR T/R")
		(attr "Datasheet Link" "https://componentsearchengine.com/Datasheets/1/SRR1260-4R7Y.pdf")
		(attr "Height" "6.5 mm")
	)

)