
CircleGhost = {ainc=0.01}

function CircleGhost.new(o)
	local o = extend(CircleGhost, o or {})	
	return Sprite.new(o)
end

function CircleGhost:init()
	self:setPower(50)
	self:setImage(TEX_boss, 32, 96, 32, 32)
	self:setColCircle(0, 12)
	self:setSpeed(2)	
end

function CircleGhost:update()
	local ai = 0
	for n=1, 30 do
		ai = ai + self.ainc
		self:setAngleInc(ai)
		yield()
	end
	
	local x = self:getX()
	local y = self:getY()
	local a = Math.rand(512)
	for n=1, 5 do
		local sprite = GhostBorder.new{parent=self}
		sprite:setPos(x, y)
		sprite:setAngle(a)
		sprite:setAngleInc(3)
		a = a + 102
	end 
	
	self:setAngleInc(0)
end

function CircleGhost:onDestroyed()
	self:drop(IT_power, 7)
	self:drop(IT_point, 5)
end

--------------------------------------------------------------------------------

GhostBorder = {parent=Nil}

function GhostBorder.new(o)
	local o = extend(GhostBorder, o or {})	
	return Sprite.new(o, SPRITE_enemyShot)
end

function GhostBorder:init()
	self:setImage(TEX_bullet, 16, 0, 16, 16)
	self:setDrawAngle(DAM_manual, 0)
end

function GhostBorder:update()
	local rad = 0
	local x0 = 0
	local y0 = 0

	while true do		
		if self.parent:isDestroyed() then
			break
		end
		local px = self.parent:getX()
		local py = self.parent:getY()

		if rad < 32 then
			rad = rad + 2

			local a = self:getAngle()
			x0 = rad * Math.sin(a)
			y0 = rad * Math.cos(a)
		else
			if px < 0 or px > 256 or py > 352 then
				break
			end
		end
	
		self:setPos(px + x0, py + y0)
		yield()
		
		for n=1, 3 do
			if self.parent:isDestroyed() then
				break
			end
			self:setPos(self.parent:getX() + x0, self.parent:getY() + y0)
			yield()		
		end
	end	
	
	self:setSpeed(3)
	self:setAngleInc(0)
end

--------------------------------------------------------------------------------

GhostPatrol = {xs={128}, ys={-32}}

function GhostPatrol.new(o)
	local o = extend(GhostPatrol, o or {})	
	return Sprite.new(o, SPRITE_enemy)
end

function GhostPatrol:init()
	self:setImage(TEX_boss, 32, 96, 32, 32)
	self:setColCircle(0, 12)
end

function GhostPatrol:update()
	self:setPos(self.xs[1], self.ys[1])

	local L = Math.min(#self.xs, #self.ys)
	for n=2, L do
		moveTo(self, self.xs[n], self.ys[n], 1, 1)
		yield()
	end
	
	self:die()
end

function GhostPatrol:onDestroyed()
	self:drop(IT_power, 1)
	self:drop(IT_point, 3)
end

--------------------------------------------------------------------------------

function ghostAnim(self)
	local lx = -999
	while true do
		local x = self:getX()
		local dw = Math.abs(self:getDrawWidth())
		
		if x > lx then
			self:setDrawSize(dw, self:getDrawHeight())
		elseif x < lx then
			self:setDrawSize(-dw, self:getDrawHeight())
		end
		
		lx = x	
		yield()
	end
end

LaserGhost = {dstx=128, dst=128, a0=128, ainc=0.25}

function LaserGhost.new(o)
	local o = extend(LaserGhost, o or {})	
	return Sprite.new(o, SPRITE_enemy)
end

function LaserGhost:init()
	self:setImage(TEX_boss, 32, 96, 32, 32)
	self:setDrawSize(128, 128)
	self:setColCircle(0, 50)
	self:setPower(1000)
end

function LaserGhost:update()
	moveTo(self, self.dstx, self.dsty, 0.5, 0.5)

	for mainloop=1, 3 do
		local a = self.a0
		
		for n=1, 90 do
			self:fire(a)
			yield()
		end
		for n=1, 512 do
			a = a + self.ainc
			self:fire(a)
			yield()
		end
		for n=1, 60 do
			self:fire(a)
			yield()
		end
	end
end

function LaserGhost:fire(a)
	Game.playSFX("laser.wav")

	local sprite = Sprite.new(Nil, SPRITE_enemyShot, 128, -16)
	sprite:setImage(TEX_bullet2, 48, 32, 16, 32)
	sprite:setDrawSize(16, 128)
	sprite:setColSegment(0, true, 0, -64+8, 8)
	sprite:setPos(self:getX(), self:getY())
	sprite:setSpeed(64)
	sprite:setAngle(a)
	sprite:setZ(-120)
end

function LaserGhost:onDestroyed()
	self:drop(IT_powerLarge, 2)
	self:drop(IT_power, 5)
	self:drop(IT_pointLarge, 3)
end

function LaserGhost:animator()
	ghostAnim(self)
end

--------------------------------------------------------------------------------

BigGhost = {}

function BigGhost.new(o)
	local o = extend(BigGhost, o or {})	
	return Sprite.new(o, SPRITE_enemy)
end

function BigGhost:init()
	self:setImage(TEX_boss, 32, 96, 32, 32)
	self:setDrawSize(128, 128)
	self:setColCircle(0, 50)
	self:setPower(1000)
end

function BigGhost:animator()
	ghostAnim(self)
end

--------------------------------------------------------------------------------

function fairyAnim(self, ty)
	local tx = 0
	while true do
		self:setImage(TEX_enemy, tx, ty, 24, 24)
		yield(10)
		
		if tx < 72 then
			tx = tx + 24
		else
			tx = 0
		end
	end
end

HailFairy = {}

function HailFairy.new(o)
	local o = extend(HailFairy, o or {})	
	return Sprite.new(o, SPRITE_enemy)
end

function HailFairy:init()
	self:setColCircle(0, 10)
	self:setPower(15)
end

function HailFairy:update()
	for n=1, 64 do
		self:fire()
		yield(10)
	end
end

function HailFairy:fire()
	Game.playSFX("shoot.wav")

	local sprite = Sprite.new(Nil, SPRITE_enemyShot, self:getX(), self:getY())
	sprite:setImage(TEX_bullet, 8, 40, 8, 8)
	sprite:setColCircle(0, 3)
	sprite:setDrawAngle(DAM_manual, 0)
	sprite:setAngle(256+Math.rand(-64, 64))
	sprite:setSpeed(2)
end

function HailFairy:animator()
	fairyAnim(self, 24)
end

function HailFairy:onDestroyed()
	self:drop(IT_power, 3)
	self:drop(IT_point, 3)
end

--------------------------------------------------------------------------------

BounceGhost = {size=8}

function BounceGhost.new(o)
	local o = extend(BounceGhost, o or {})	
	return Sprite.new(o)
end

function BounceGhost:init()
	self:setImage(TEX_boss, 32, 96, 32, 32)
	self:setDrawSize(self.size, self.size)
	self:setColCircle(0, self.size * 0.45)
	self:setPower(600)
end

function BounceGhost:update()
	yield(60)
	self:setSpeed(0)
	
	local spdx = 1.0 + Math.rand(1)
	local spdy = 1.0 + Math.rand(1.5)	
	
	while true do
		local x = self:getX()
		local y = self:getY()
		local pad = Math.max(self.size/4, 8)
		
		if x < pad     and spdx < 0 then spdx = -spdx end
		if x > 256-pad and spdx > 0 then spdx = -spdx end
		if y < pad     and spdy < 0 then spdy = -spdy end
		if y > 352-pad and spdy > 0 then spdy = -spdy end
		
		self:setPos(x + spdx, y + spdy)
	
		yield(1)
	end
end

function BounceGhost:onDestroyed()
	self:drop(ItemType.IT_life, 1)
	self:drop(ItemType.IT_fullPower, 1)
	self:drop(ItemType.IT_pointLarge, 5)
	self:drop(ItemType.IT_point, 15)
end

function BounceGhost:onCollision(s2t, s2p, s2x, s2y)
	self.size = Math.min(255, self.size + s2p / 2)
end

function BounceGhost:animator()
	local lx = -999
	while true do
		local x = self:getX()
		
		self:setColCircle(0, self.size * 0.45)
		if x > lx then
			self:setDrawSize(self.size, self.size)
		elseif x < lx then
			self:setDrawSize(-self.size, self.size)
		end
		
		lx = x	
		yield()
	end
end

--------------------------------------------------------------------------------

function patrol01()
	for n=1, 6 do
		local ghost = GhostPatrol.new{
			xs={ 24, 232, 128},
			ys={368, 160, -48}}
		yield(40)
	end
end

function patrol02()
	for n=1, 6 do
		local ghost = GhostPatrol.new{
			xs={288, -32},
			ys={160, 320}}
		yield(40)
	end
end

function patrol03()
	for n=1, 6 do
		local ghost = GhostPatrol.new{
			xs={24, 232,  24},
			ys={-48, 160, 368}}
		yield(40)
	end
end

function laserDisturbance()
	for n=1, 6 do
		local ghost = CircleGhost.new{ainc=-0.01}
		local r = Math.rand(0, 256)
		if r <= 127 then
			ghost:setPos(128 + r, -32)
			ghost:setAngle(320)
			ghost:setAngleInc(-0.02)
		else
			ghost:setPos(288, r-128)			
			ghost:setAngle(356)
			ghost:setAngleInc(-0.02)
		end
		
		yield(90)
	end
end

function main()
	Game.setBGM("bgm.wv")

	local ghost = CircleGhost.new{ainc=0.01}
	ghost:setPos(128, -32)
	yield(120)
	local ghost = CircleGhost.new{ainc=-0.01}
	ghost:setPos(24, -32)
	yield(30)
	local ghost = CircleGhost.new{ainc=0.01}
	ghost:setPos(224, -32)
	yield(45)
	local ghost = CircleGhost.new{ainc=-0.01}
	ghost:setPos(170, -32)
	
	Game.addThread("patrol01")
	yield(30)
	Game.addThread("patrol02")
	yield(60)
	
	local bigboo = LaserGhost.new{dstx=48, dsty=80}
	bigboo:setPos(-64, 64)
	bigboo.onDestroyed = function(self)
		self:drop(IT_powerLarge, 2)
		self:drop(IT_power, 5)
		self:drop(IT_pointLarge, 3)
		self:drop(IT_bomb, 1)
	end

	yield(400)
	Game.addThread("laserDisturbance")
	
	while not bigboo:isDestroyed() do
		yield()
	end
	yield(60)
	
	local y0 = Math.rand(-64, 64)
	for n=1, 4 do
		for y=y0, 416, 144 do	
			local bigboo = BigGhost.new()
			bigboo:setPos(-64, y)
			bigboo:setAngle(128)
			bigboo:setSpeed(1.5)
		end
		yield(60)	
		for y=y0-72, 416, 144 do	
			local bigboo = BigGhost.new()
			bigboo:setPos(320, y)
			bigboo:setAngle(384)
			bigboo:setSpeed(1.5)
		end
		yield(120)
	end

	for y=48, 128, 16 do
		local sprite = HailFairy.new()
		sprite:setPos(-12, y)
		sprite:setAngle(128)
		sprite:setSpeed(2)
		yield(10)
	end
	yield(60)
	for y=16, 128, 16 do
		local sprite = HailFairy.new()
		sprite:setPos(268, y)
		sprite:setAngle(384)
		sprite:setSpeed(2)
		yield(10)
	end
	yield(90)
	for y=48, 128, 16 do
		local sprite = HailFairy.new()
		sprite:setPos(-12, y)
		sprite:setAngle(192)
		sprite:setSpeed(1.5)
		yield(10)
	end
	yield(30)
	
	Game.addThread("patrol01")
	Game.addThread("patrol03")

	local ghost = CircleGhost.new{ainc=-0.01}
	ghost:setPos(32, -32)
	yield(30)
	local ghost = CircleGhost.new{ainc=0.01}
	ghost:setPos(216, -32)
	yield(40)
	local ghost = CircleGhost.new{ainc=-0.01}
	ghost:setPos(64, -32)
	yield(30)
	local ghost = CircleGhost.new{ainc=0.01}
	ghost:setPos(192, -32)
	
	for y=16, 128, 16 do
		local sprite = HailFairy.new()
		sprite:setPos(268, y)
		sprite:setAngle(384)
		sprite:setSpeed(2)
		yield(10)
	end

	yield(120)	
	local bigboo = LaserGhost.new{dstx=48, dsty=80, ainc=.17}
	bigboo:setPower(250)
	bigboo:setPos(-64, 64)
		
	local bigboo2 = LaserGhost.new{dstx=208, dsty=80, a0=384, ainc=-.17}
	bigboo2:setPower(250)
	bigboo2:setPos(320, 64)
	bigboo2:setDrawSize(-128, 128)	
	yield(120)

	while not bigboo:isDestroyed() or not bigboo2:isDestroyed() do
		for y=48, 80, 32 do
			local sprite = HailFairy.new()
			sprite:setPos(-12, y)
			sprite:setAngle(128)
			sprite:setSpeed(1)
			yield(10)
		end
		yield(120)
		for y=32, 64, 32 do
			local sprite = HailFairy.new()
			sprite:setPos(268, y)
			sprite:setAngle(384)
			sprite:setSpeed(1)
			yield(10)
		end
		yield(240)
	end		
	yield(180)
	
	local bigboo = BounceGhost.new()
	bigboo:setPos(128, -32)
	while not bigboo:isDestroyed() do
		yield(10)
	end

	Game.setBGM("")
	yield(240)
	Game.setBGM("yuyuko.wv")
	yield(240)
	
	local boss = Yuyucow.new()
	boss.onDestroyed = function(self)		
		Game.addThread("endStage")
	end
end

function endStage()
	yield(300)
	Game.endStage()
end
