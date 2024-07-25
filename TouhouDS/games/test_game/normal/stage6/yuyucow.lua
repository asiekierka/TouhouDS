
function moveTo(self, xend, yend, xspd, yspd)
	self:setSpeed(0)	

	while Math.abs(xend-self:getX()) > xspd or Math.abs(yend-self:getY()) > yspd do
		local x = self:getX()
		local y = self:getY()
		
		if x > xend + xspd then x = x - xspd end
		if x < xend - xspd then x = x + xspd end
		if y > yend + yspd then y = y - yspd end
		if y < yend - yspd then y = y + yspd end
		
		self:setPos(x, y)
		
		yield()
	end
end

-------------------------------------------------------------------------------

Yuyucow = {}

function Yuyucow.new(o)
	local o = extend(Yuyucow, o or {})	
	return Boss.new(o)
end

function Yuyucow:init()
	self:setPower(300)
	self:setPos(128, -32)
	self:setImage(TextureType.TEX_boss, 0, 0, 42, 42)
	self:setSpeed(1)
	
	self:setName("Yuyucow")
	
	self:addSpellcard("card1", 400,  90, "Windmill")
	self:addSpellcard("card2", 1000, 120, "Criss-Cross")	
	self:addSpellcard("card3", 1000, 120, "Butterfly Pyrotechnics")
	self:addSpellcard("card4", 1250, 120, "Lickitung")
	self:addSpellcard("card5", 1000, 120, "Stratus Rhopalocera")
end

function Yuyucow:card1()
	self:setInvincibleTime(9999)
	moveTo(self, 128, 140, 1, 1)
	self:setInvincibleTime(0)

	local a0 = 0
	local aspd = 0
	local aspdinc = .002
	
	while true do
		local x = self:getX()
		local y = self:getY()
		
		for a=a0, a0+47, 48 do --decrease stepsize for difficulty
			local sprite = self:fireButterfly(0, x, y, a)
			sprite:setSpeedInc(0.05)
		end
		
		a0 = a0 + 64 + aspd

		if aspd < -1.0 or aspd > 1.0 then
			aspdinc = -aspdinc
		end
		aspd = aspd + aspdinc
		
		yield()
	end
end

function Yuyucow:card2()
	self:setInvincibleTime(9999)
	moveTo(self, 128, 64, 1, 1)	
	self:setInvincibleTime(0)

	local a0 = 0
	local frame = 0
	local dir = 1
	
	while true do
		local x = self:getX()
		local y = self:getY()
		
		Game.playSFX("shoot.wav")
		
		local ai = dir * 0.5
		for a=a0, a0+31, 16 do		
			local sprite = Sprite.new(Nil, SpriteType.SPRITE_enemyShot, x, y)
			if dir < 0 then			
				sprite:setImage(TextureType.TEX_boss, 64, 96, 32, 32)
			else
				sprite:setImage(TextureType.TEX_boss, 96, 96, 32, 32)
			end
			sprite:setColCircle(0, 6)
			sprite:setAngleInc(ai)
			sprite:setAngle(a)
			sprite:setSpeedInc(0.03)
			sprite:setSpeed(0.5)			
		end
		
		a0 = a0 + 32
		
		frame = frame + 1
		if frame > 180 then
			frame = 0
			a0 = a0 + Math.rand(0, 4)
			dir = -dir
		end
	
		yield()
	end
end

function Yuyucow:card3()
	self:setInvincibleTime(9999)
	moveTo(self, 128, 64, 1, 1)	
	self:setInvincibleTime(0)

	while true do
		local x = self:getX()
		local y = self:getY()
		
		Game.playSFX("shoot.wav")
		
		local seeker = Seeker.new()
		seeker:setPos(x, y)
		seeker:setAngle(128)
		yield(180)
		
		local seeker = Seeker.new()
		seeker:setPos(x, y)
		seeker:setAngle(-128)	
		yield(180)
	end
end

function Yuyucow:card4()
	self:setInvincibleTime(9999)
	moveTo(self, 128, 64, 1, 1)	
	self:setInvincibleTime(0)

	local cx = 64
	local cxinc = 2
	local cy = 128
	
	while true do
		local x = self:getX()
		local y = self:getY()
		
		local splitter = Splitter.new{dstx=cx, dsty=cy}
		splitter:setPos(x, y)
		
		if cx < 8 or cx >= 248 then
			cxinc = -cxinc
		end
		cx = cx + cxinc
		
		local sprite = Sprite.new(Nil, SpriteType.SPRITE_enemyShot,
			Math.rand(32, 224), -32)			
		sprite:setImage(TextureType.TEX_bullet2, 0, 0, 32, 32)
		sprite:setDrawAngle(DrawAngleMode.DAM_manual, 0)
		sprite:setColCircle(0, 8)
		sprite:setSpeed(1)
		sprite:setAngle(Math.rand(128, 384))
		
		yield(8)
	end
end

function Yuyucow:card5()
	self:setInvincibleTime(9999)
	moveTo(self, 128, 64, 1, 1)	
	self:setInvincibleTime(0)
	self:setSpeed(0.05)
	
	local step = 48
	local stepinc = -1
	
	while true do
		local x = self:getX()
		local y = self:getY()
		
		if y < 28 then
			self:setAngle(256)
		end
		if y > 100 then
			self:setAngle(0)
		end
		
		local yy = y + 32
		local ainc = 128 / step
		
		for xx=0, 256, step do
			local cloud = Cloud.new()
			cloud:setPos(xx, yy)
			cloud:setAngle(0)
			cloud:setSpeedInc(0.005)
			cloud:setAngleInc(ainc)
		end
		
		if step <= 32 or step >= 64 then
			stepinc = -stepinc
		end
		step = step + stepinc
						
		yield(10)
	end
end

function Yuyucow:fireButterfly(type, x, y, angle)
	local tx = 96
	if type == 1 then
		tx = 64
	end

	local sprite = Sprite.new(Nil, SpriteType.SPRITE_enemyShot, x, y)			
	sprite:setImage(TextureType.TEX_boss, tx, 96, 32, 32)
	sprite:setColCircle(0, 6)
	sprite:setSpeed(1)
	sprite:setAngle(angle)
	sprite:advance(12)
	return sprite
end

-------------------------------------------------------------------------------

Seeker = {}

function Seeker.new(o)
	local o = extend(Seeker, o or {})	
	return Sprite.new(o, SpriteType.SPRITE_enemyShot)
end

function Seeker:init()
	self:setImage(TextureType.TEX_boss, 96, 96, 32, 32)
	self:setColCircle(0, 6)
	self:setSpeed(1.5)
	self:advance(12)
end

function Seeker:update()	
	local frame = 0
	while frame < 60 do
		local angle0 = self:getAngle()
		local angle1 = Math.angleBetween(self:getX(), self:getY(),
			player:getX(), player:getY())
	
		self:setAngle(Math.max(angle0-0.5, Math.min(angle0+0.5, angle1)))
		frame = frame + 1	
		yield()
	end
	
	local a0 = Math.rand(0, 16)
	local x = self:getX()
	local y = self:getY()
	
	local m = 0
	for n=1, 4 do
		for a=a0, a0+511, 64 do
			for b=a, a+63, 16 do
				self:fire(m, x, y, b)
			end			
			yield()
		end

		a0 = a0 + 4

		if m == 0 then
			m = 1
		else
			m = 0
		end		
	end
	
	self:die()
end

function Seeker:fire(type, x, y, angle)
	Game.playSFX("shoot.wav")

	local tx = 96
	if type == 1 then
		tx = 64
	end

	local sprite = Sprite.new(Nil, SpriteType.SPRITE_enemyShot, x, y)			
	sprite:setImage(TextureType.TEX_boss, tx, 96, 32, 32)
	sprite:setColCircle(0, 6)
	sprite:setSpeed(2)
	sprite:setAngle(angle)
	return sprite
end

-------------------------------------------------------------------------------

Splitter = {dstx=128, dsty=128, gen=1}

function Splitter.new(o)
	local o = extend(Splitter, o or {})	
	return Sprite.new(o, SpriteType.SPRITE_enemyShot)
end

function Splitter:init()
	local tx = 96
	if self.gen == 0 then
		tx = 64
	end
	
	self:setImage(TextureType.TEX_boss, tx, 96, 32, 32)
	self:setColCircle(0, 6)
	self:setSpeed(2)
end

function Splitter:update()		
	local angle1 = Math.angleBetween(self:getX(), self:getY(), self.dstx, self.dsty)
	self:setAngle(angle1)

	yield(self.gen * 60)	
	self:setAngleInc(16)
	yield(Math.rand(40, 72))
	self:setAngleInc(0)
	
	if self.gen <= 0 then
		return
	end
	
	local x = self:getX()
	local y = self:getY()
	
	for a=0, 511, 192 do
		self:fire(x, y, a)
	end
	
	self:die()
end

function Splitter:fire(x, y, a)
	Game.playSFX("shoot.wav")

	local splitter = Splitter.new{dstx=self.dstx, dsty=self.dsty, gen=self.gen-1}
	splitter:setPos(x, y)
	splitter:setAngle(a)
	return cloud
end

-------------------------------------------------------------------------------

Cloud = {}

function Cloud.new(o)
	local o = extend(Cloud, o or {})	
	return Sprite.new(o, SpriteType.SPRITE_enemyShot)
end

function Cloud:init()
	self:setImage(TextureType.TEX_boss, 96, 96, 32, 32)
	self:setColCircle(0, 6)
	self:setSpeed(2)
end

function Cloud:update()		
	yield(Math.rand(60, 180))
	self:setSpeedInc(-.01)
	self:setAngleInc(0)
end

-------------------------------------------------------------------------------
