local img = Graphics.loadImage("noob.png")
local font = Graphics.loadFont("emotion", "emotion.fnt", "emotion.gs", 32.0, 39.0)

while true do
    RenderManager.beginFrame()

    Graphics.clear(Colors.DarkGray)
	Graphics.drawImage(0, 0, img)
    Graphics.drawString(font, "Hello World", -320, 240, 64)

    RenderManager.endFrame()
end

Graphics.freeFont(font)
Graphics.freeImage(img)