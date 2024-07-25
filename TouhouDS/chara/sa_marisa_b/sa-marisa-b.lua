
SA_Marisa_B = {opts={}, mode=0, switchCooldown=0}

function SA_Marisa_B:init()
	self.lastX = self:getX()
	self.lastY = self:getY()		

	for n=1, 4 do
		self.opts[n] = Sprite.new(Nil, SpriteType.SPRITE_default, self.lastX, self.lastY)
		self.opts[n]:setVisible(false)
	end
end

function SA_Marisa_B:update()
	self:onModeChanged()

	while true do
		local x = self:getX()
		local y = self:getY()
		local level = self:getAttackLevel()
				
		local focusPressed = self:isButtonPressed(Keys.VK_focus)
		local firePressed = self:isButtonPressed(Keys.VK_fire)
					
		if self.switchCooldown > 0 and focusPressed and firePressed then			
			self.mode = self.mode + 1
			if self.mode > 4 then
				self.mode = 0
			end
			
			self.switchCooldown = 0
						
			self:onModeChanged()
		end 

		for n=1, 4 do
			self.opts[n]:setVisible(n <= level)			
		end

		if focusPressed or firePressed then
			self.switchCooldown = self.switchCooldown - 1
		else
			self.switchCooldown = 2
		end

		if self.mode == 1 then
			self.opts[1]:setPos(x-8,  y+24)
			self.opts[2]:setPos(x+8,  y+24)
			self.opts[3]:setPos(x-24, y+24)
			self.opts[4]:setPos(x+24, y+24)		
		elseif self.mode == 2 then
			self.opts[1]:setPos(x-16, y-16)
			self.opts[2]:setPos(x+16, y-16)
			self.opts[3]:setPos(x-16, y-16)
			self.opts[4]:setPos(x+16, y-16)		
		elseif self.mode == 3 then
			self.opts[1]:setPos(x, y-40)
			self.opts[2]:setPos(x, y+40)
			self.opts[3]:setPos(x, y-20)
			self.opts[4]:setPos(x, y+20)		
		elseif self.mode == 4 then
			self.opts[1]:setPos(x,    y+40)
			self.opts[2]:setPos(x-16, y+24)
			self.opts[3]:setPos(x+16, y+24)
			self.opts[4]:setPos(x,    y+24)		
		else
			self.opts[1]:setPos(x-24, y-8)
			self.opts[2]:setPos(x+24, y-8)
			self.opts[3]:setPos(x-24, y+8)
			self.opts[4]:setPos(x+24, y+8)		
		end
		
		yield(2)		
	end
end

function SA_Marisa_B:onModeChanged()
	local dx = self.mode * 8
	local dy = 0
	
	if self.mode >= 3 then
		dx = dx - 24
		dy = 8
	end

	for n=1, 4 do
		self.opts[n]:setImage(TextureType.TEX_player, 96 + dx, 96 + dy, 8, 8)		
	end
	
	if self.mode == 0 then
		self.opts[1]:setAngle(6) 
		self.opts[2]:setAngle(-6) 
		self.opts[3]:setAngle(6) 
		self.opts[4]:setAngle(-6)
	elseif self.mode == 1 then
		self.opts[1]:setAngle(-8) 
		self.opts[2]:setAngle(8) 
		self.opts[3]:setAngle(-16) 
		self.opts[4]:setAngle(16)
	elseif self.mode == 2 then
		self.opts[1]:setAngle(-20) 
		self.opts[2]:setAngle(20) 
		self.opts[3]:setAngle(-28) 
		self.opts[4]:setAngle(28)
	elseif self.mode == 3 then
		self.opts[1]:setAngle(128) 
		self.opts[2]:setAngle(-128) 
		self.opts[3]:setAngle(-128) 
		self.opts[4]:setAngle(128)
	elseif self.mode == 4 then
		self.opts[1]:setAngle(256) 
		self.opts[2]:setAngle(256-16) 
		self.opts[3]:setAngle(256+16) 
		self.opts[4]:setAngle(256)
	end
	
end

function SA_Marisa_B:fire()
	local x = self:getX()
	local y = self:getY()
	local level = self:getAttackLevel()
	local pow1 = self:getAttackPower1()
	local pow2 = self:getAttackPower2()

	self:fire1(x, y, pow1, 0)
	for n=1, 4 do
		local opt = self.opts[n]
		if opt:isVisible() then
			self:fire2(opt:getX(), opt:getY(), pow2, opt:getAngle())
		end
	end
	
	yield(6)
end

function SA_Marisa_B:fire1(x, y, pow, angle)
	local sprite = Sprite.new(Nil, SpriteType.SPRITE_playerShot, x, y)
	sprite:setImage(TextureType.TEX_player, 0, 104, 24, 24)
	sprite:setColCircle(0, 10)
	sprite:setPower(pow)
	sprite:setAngle(angle)	
	sprite:setSpeed(12)	
end

function SA_Marisa_B:fire2(x, y, pow, angle)
	local m = 24
	if self.mode == 1 then m = 48 end
	if self.mode == 2 then m = 0  end
	if self.mode == 3 then m = 72 end
	if self.mode == 4 then m = 24 end

	local sprite = Sprite.new(Nil, SpriteType.SPRITE_playerShot, x, y)
	sprite:setImage(TextureType.TEX_player, m, 104, 24, 24)
	sprite:setColCircle(0, 10)
	sprite:setPower(pow)
	sprite:setAngle(angle)	
	sprite:setSpeed(12)	
end
