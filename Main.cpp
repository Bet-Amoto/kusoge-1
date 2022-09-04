
# include <Siv3D.hpp> // OpenSiv3D v0.6.2

String Btos(BigInt num) {
	const String kasu = U" KMBTabcdefghijklmnopqrstuvwxyz";
	int keta = Log10(num).asFloat();
	if(keta < 4) return U"{}"_fmt(num);
	float hy = (num / (10_big).pow(keta-3)).asFloat();
	switch (keta%3)
	{
	case 0:
		return U"{}{}"_fmt(hy,kasu[Max(0, (keta - 1) / 3)]);
	case 1:
		return U"{:.2f}{}"_fmt(hy / 100, kasu[Max(0, (keta - 1) / 3)]);

	case 2:
		return U"{:.1f}{}"_fmt(hy / 10, kasu[Max(0, (keta - 1) / 3)]);

	default:
		return U"error!";
	};
}

BigInt ftoi(BigFloat bf) {
	String str = bf.str();
	return BigInt(str.split('.')[0]);

}

class Coin
{
public:
	Coin(BigInt m, Vec2 p) {
		money = m;
		pos = p;
		tpos = Vec2();
		tim = 0;
		speed = Vec2(Random(-150, 150), Random(50, 250));
	}
	void updata(Vec2 atp) {
		tim += Scene::DeltaTime();
		if (tim < 4) {
			pos += speed * Scene::DeltaTime();
			speed.x *= 0.99;
			speed.y += gl * Scene::DeltaTime();
			if (pos.x >= 800 - size)speed.x = -1 * Abs(speed.x);
			if (pos.x <= 0 + size)speed.x = Abs(speed.x);
			if (pos.y >= 500 - size)speed.y = -0.4 * Abs(speed.y);
			if (AbsDiff(pos.x, atp.x) < 50&& pos.y>400)tim = 4;
		}
		else {
			if (tpos.isZero())tpos = pos;
			double ttt = EaseOutCubic(Clamp(tim - 4,0.0,1.0));
			pos = tpos.lerp(atp,ttt);
		}
	}
	void draw() {
		coincir().draw(Palette::Yellow);
	}
	Circle coincir() {
		return Circle(pos, size);
	}
	BigInt getmoney() {
		if (die)return 0_big;
		die = true;
		return money;
	}
	bool isdie() {
		return die;
	}
private:
	Vec2 pos, tpos, speed;
	double gl = 400;
	double tim;
	double size = 10;
	bool die = false;
	BigInt money;
};

class NumBall {
public:
	NumBall(BigInt thp, BigInt tmaxhp, double tsize,BigInt tmoney, bool move = true) {
		initball(thp, tmaxhp, tsize,tmoney,move);

	}
	NumBall(BigInt tmaxhp, BigInt tmoney, bool move = true) {
		initball(tmaxhp,tmoney,move);
	}
	void initball(BigInt thp,BigInt tmaxhp,double tsize,BigInt tmoney, bool move = true) {
		coins = Array<Coin>();
		hp = thp;
		maxhp = tmaxhp;
		size = tsize;
		money = tmoney;
		gl = Random(60.0, 120.0);
		pos = Vec2(Random(0,1) == 1 ? -200 : 800, Random(50, 100));
		speed = Vec2(pos.x>0 ? Random(-250,-80):Random(80,250), Random(-10,10));
		moving = move;
	}
	void initball(BigInt tmaxhp, BigInt tmoney, bool move = true) {
		hp = tmaxhp * Random(300, 1000) / 1000;
		initball(hp, tmaxhp,Random(minsize,maxsize),tmoney);
	}
	void updata() {
		if (not moving)return;
		pos += speed*Scene::DeltaTime();
		speed.y += gl*Scene::DeltaTime();
		if (pos.x >= 800 - size)speed.x = -1 * Abs(speed.x);
		if (pos.x <= 0 + size)speed.x = Abs(speed.x);
		if (pos.y >= 500 - size)speed.y = -1 * Abs(speed.y);
	}
	void draw() {
		if (not moving)return;
		ballcir().draw(HSV(120- 120 * (Min((BigFloat)1.0, (BigFloat)hp / ((BigFloat)maxhp * 0.8))).asFloat()));
		FontAsset(U"numball")(Btos(hp)).drawAt(pos);
	}
	Circle ballcir() {
		return Circle(pos, size);	}

	void damage(BigInt damage) {
		hp -= damage;
		if (hp <= 0) {
			moving = false;
			makecoin();
		}
	}

	bool ismove() {
		return moving;
	}

	Array<Coin> getcoins() {
		return coins;
	}
private:
	void makecoin() {
		int kazu = Random(1, Min(money, 5_big).asInt32());
		for (auto i : step(kazu)) {
			if (i == kazu - 1)coins << Coin(money - (money / kazu) * (kazu - 1), pos);
			else coins << Coin(money / kazu,pos);
		}
	}
	Array<Coin> coins;
	int maxsize = 100;
	int minsize = 40;
	BigInt hp,maxhp,money;
	Vec2 pos, speed;
	double size,gl;
	bool moving;
};

class Bullet {
public:
	Bullet(BigInt damage, Vec2 pos, Vec2 speed) {
		this->damage = damage;
		this->pos = pos;
		this->speed = speed;
		die = false;
	}
	void updata() {
		if (die)return;
		pos += speed * Scene::DeltaTime();
		if (pos.y < 0 - size)die = true;
	}
	void draw() {
		if (die)return;
		ballcir().draw(Palette::Darkolivegreen);
	}
	Circle ballcir() {
		return Circle(pos, size);
	}
	bool isdestroy() {
		return die;
	}
	BigInt hitdamage() {
		return damage;
	}
	bool ishit(Circle enemy) {
		if (ballcir().intersects(enemy)) {
			die = true;
			return true;
		}
		return false;
	}
private:
	bool die;
	BigInt damage;
	Vec2 pos,speed;
	double size = 10;
};

class Attacker
{
public:
	Attacker() {
		pos = Vec2();
	}
	void updata() {
		pos = Vec2(Clamp(Cursor::Pos().x, 50, 750), 440);
	}
	void draw() {
		Circle(pos.movedBy(0,20),40).draw(Palette::Purple);
		Rect(Arg::center((int)pos.x,(int)pos.y-15),40,20).draw(Palette::Purple);
	}
	Vec2 getpos() {
		return pos;
	}
	void setpos(Vec2 p) {
		pos = p;
	}
	Array<Bullet> makebullet(BigInt powor,double speed) {
		Array<Bullet> bullets;
		bullets << Bullet(powor, getpos(), Vec2(0, speed));
		return bullets;
	}
	Circle body() {
		return Circle(pos.movedBy(0, 20), 40);
	}
private:
	Vec2 pos;
};

BigInt attacknum(int Lv) {
	if (Lv <= 20)return (BigInt)Lv;
	return ftoi(20_bigF*Pow(1.1_bigF,Lv-20));
}

BigInt attackprime(int Lv){
	return 8_big +(Pow((BigInt)Lv,2));
}

int atcountnum(int Lv){
	return Lv + 2;
}

BigInt atcountprime(int Lv) {
	return 10_big + ftoi((Pow(5_bigF,Log(Lv*Lv/2))));
}

double speednum(int Lv) {
	return Max(-300 * Pow(1.04, Lv), -2000.0);
}

BigInt speedprime(int Lv) {
	return 30_big + ftoi(Pow(6_bigF, Log((double)Lv*Lv/2)));
}

BigInt maxnumball(int lebel) {
	return 6_big * Pow((BigInt)lebel, 1.5+Log10(lebel/2));
}
void Main()
{
	const double cooltime = 1.5;
	int maxballs = 5;
	int atlv = 1, colv = 1, splv = 1;
	BigInt mymoney = 0;
	int lebel = 1;
	int killcount = 0;
	bool title = true;
	FontAsset::Register(U"numball", 30, Typeface::Medium);
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });
	Circle me(400, 460, 40);
	Array<NumBall> balls;
	Array<Coin> coins;
	double t = 1.5,tt = 0;
	Array<Bullet> attacks;
	Attacker att = Attacker();
	while (System::Update())
	{
		Rect(0, 500, 800, 100).draw(Palette::Greenyellow);
		FontAsset(U"numball")(Btos(mymoney)).drawAt(700,50, Palette::Black);
		if (title) {
			if (att.body().leftPressed()) {
				title = false;
				t = 0;
				tt = 0;
				attacks.clear();
				balls.clear();
			}
			FontAsset(U"numball")(U"title").drawAt(Scene::Center().movedBy(0, -150), Palette::Black);
			FontAsset(U"numball")(U"level {}"_fmt(lebel)).drawAt(Scene::Center().movedBy(0, -120), Palette::Black);
			FontAsset(U"numball")(U"攻撃力").drawAt(Scene::Center().movedBy(-150, -70), Palette::Black);
			FontAsset(U"numball")(U"弾の数").drawAt(Scene::Center().movedBy(0,-70),Palette::Black);
			FontAsset(U"numball")(U"弾速").drawAt(Scene::Center().movedBy(150,-70),Palette::Black);
			FontAsset(U"numball")(U"{}->{}"_fmt(Btos(attacknum(atlv)),Btos(attacknum(atlv+1)))).drawAt(Scene::Center().movedBy(-150, -40), Palette::Black);
			FontAsset(U"numball")(U"{}->{}"_fmt(atcountnum(colv), atcountnum(colv+1))).drawAt(Scene::Center().movedBy(0, -40), Palette::Black);
			FontAsset(U"numball")(U"{}->{}"_fmt(Abs(floor(speednum(splv))), Abs(floor(speednum(splv+1))))).drawAt(Scene::Center().movedBy(150, -40), Palette::Black);

			if (SimpleGUI::Button(U"{}"_fmt(Btos(attackprime(atlv))), Scene::Center().movedBy(-170, -10)))
			{
				if (mymoney >= attackprime(atlv)) {
					mymoney -= attackprime(atlv);
					atlv++;
				}
			}
			if (SimpleGUI::Button(U"{}"_fmt(Btos(atcountprime(colv))), Scene::Center().movedBy(-20, -10)))
			{
				if (mymoney >= atcountprime(colv)) {
					mymoney -= atcountprime(colv);
					colv++;
				}
			}
			if (SimpleGUI::Button(U"{}"_fmt(Btos(speedprime(splv))), Scene::Center().movedBy(130, -10)))
			{
				if (mymoney >= speedprime(splv)) {
					mymoney -= speedprime(splv);
					splv++;
				}
			}
			for (auto& co : coins) {
				Circle p = att.body();
				co.updata(Vec2(p.x, p.y));
				if (att.body().contains(co.coincir()))mymoney += co.getmoney();
				co.draw();
			}
			att.setpos(Vec2(400, 440));
			att.draw();
			coins.remove_if([](Coin c)->bool {return c.isdie(); });
			continue;
		}
		t += Scene::DeltaTime();
		tt += Scene::DeltaTime();
		if (t > cooltime and balls.size()<3+log(lebel)/2 and killcount+balls.size()< 10 + log(lebel) * 2) {
			balls << NumBall(maxnumball(lebel),(BigInt)Random((int)Pow(lebel,1.5),(int)Pow(lebel,2.5)));
			t = 0;
		}
		if (tt > 1.0/atcountnum(colv)) {
			attacks.insert(attacks.end(),att.makebullet(attacknum(atlv), speednum(splv)).front());
			tt = 0;
		}
		for (auto& ba:balls) {
			if (not ba.ismove())continue;
			ba.updata();
			for (auto& at : attacks) {
				if (at.ishit(ba.ballcir())) {
					ba.damage(at.hitdamage());
				}
			}
			if (not ba.ismove()) {
				killcount++;
				Array<Coin> tmp = ba.getcoins();
				coins.insert(coins.end(), tmp.begin(),tmp.end());
			}
			if (ba.ballcir().intersects(att.body())) {
				title = true;
				killcount = 0;
				break;
			}
			ba.draw();
		}
		for (auto& co : coins) {
			Circle p = att.body();
			co.updata(Vec2(p.x,p.y));
			if (att.body().contains(co.coincir()))mymoney += co.getmoney();
			co.draw();
		}
		for (auto& at : attacks) {
			at.updata();
			at.draw();
		}
		if (killcount >= 10+log(lebel)*2) {
			lebel++;
			killcount=0;
			title = true;
		}
		coins.remove_if([](Coin c)->bool {return c.isdie(); });
		balls.remove_if([](NumBall b)->bool {return !b.ismove(); });
		attacks.remove_if([](Bullet a)->bool {return a.isdestroy(); });
		att.updata();
		att.draw();
	}
}
