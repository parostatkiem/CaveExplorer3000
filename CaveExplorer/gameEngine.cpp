#include "config.h"



unsigned int tmp;
unsigned int Turns;
int CaveSegments;


int mathem::CalculateDistance(COORD a, COORD b)
{
	return sqrt(pow((a.X - b.X), 2) + pow((a.Y - b.Y), 2));
}

int mathem::RandomInt(int min, int max)
{
	int r;
	const unsigned int range = 1 + max - min;
	const unsigned int buckets = RAND_MAX / range;
	const unsigned int limit = buckets * range;


	do
	{
		r = rand();
	} while (r >= limit);

	return min + (r / buckets);
}


void menu::ShowMenu()
{

	system("CLS");
	unsigned char menuIndex = 0;
	short colors[5] = { 0 };
	int i = 0;
	unsigned char c;
	gameEngine::DrawLogo();

	do
	{
		if (TheGameEngine.GameState != 0 && TheGameEngine.GameState != 1)
			return;
		for (i = 0; i < 5; i++)
		{
			if (menuIndex == i)
			{
				colors[i] = color_menu_active;
			}
			else
			{
				colors[i] = color_menu;
			}


		}
		if (TheGameEngine.GameState != 1 && !TheGameEngine.GameSaveLoad || !isSaved())
		{
			colors[1] = 0x08;
		}



		console::drawMenuItem(17, 7, colors[0], menuLang.findKey("Start_New_Game").c_str());
		if (isSaved() && TheGameEngine.GameSaveLoad  &&  TheGameEngine.GameState == 0)
		{


			console::drawMenuItem(16, 12, colors[1], menuLang.findKey("Continue_Saved_Game").c_str());
		}
		else
		{


			console::drawMenuItem(22, 12, colors[1], menuLang.findKey("Continue_Saved_Game").c_str());
		}



		console::drawMenuItem(24, 17, colors[2], menuLang.findKey("Options").c_str());

		console::drawMenuItem(21, 22, colors[3], menuLang.findKey("End_Game").c_str());



		c = console::getKey();


		if (c == 72) //gora
		{
			if (menuIndex >0)
			{
				menuIndex--;
			}
		}

		if (c == 80)//dol
		{
			if (menuIndex < (4 - 1))
			{
				menuIndex++;
			}
		}




		if (c == 13)//enter
		{
			switch (menuIndex)
			{
			case 0: //nowa gra

				system("CLS");

				TheGameEngine.GameState = 1;
				TheGameEngine.CurrentLevel = 0;
				Turns = 0;
				player.hp = PlayerStartHp; // 3 dla testow
				player.damage = PlayerStartDamage;
				player.maxhp = PlayerStartHp;
				player.exp = 0; //start exp
				player.level = 1;
				TheGameEngine.EnemiesKilled = 0;
				map::InitializeLevel(TheGameEngine.CurrentLevel);
				


				player.getKey();

				return;
				break;
			case 1: //kontynuuj

				if (TheGameEngine.GameState == 1)
				{
					//gra sie toczy, zamknij menu
					system("CLS");
					gameEngine::RefreshMap();
					gameEngine::RefreshGui();
					return;
				}
				else
				{
					//wczytaj gre
					if (isSaved())
					{
						if (!loadGame())
						{
							system("CLS");
							console::drawMenuItem(25, 13, color_block_enemy, menuLang.findKey("Fatal_Error").c_str()); //error
							system("pause");
							system("CLS");
							break;
						}
						TheGameEngine.GameState = 1;
						system("CLS");
						gameEngine::RefreshMap();
						gameEngine::RefreshGui();
						player.getKey();
					}

				}




				break;

			case 2: //opcje
				tmp = TheGameEngine.GameState; //zapis obecnego TheGameEngine.GameState do tymczasowej zmiennej
				TheGameEngine.GameState = 2;
				gameEngine::ShowOptions(tmp);
				gameEngine::DrawLogo();  //ponowne wy�wietlenie grafiki logo
				break;

			case 3: //wyjdz

				if (TheGameEngine.area[0][0] == block_static && TheGameEngine.GameSaveLoad || !TheGameEngine.GameSaveLoad)
					saveGame(); //zapisanie gry na wyjsciu jesli wczytana jest jakas mapa lub zmieniono opcje
				exit(0);  //ostateczne wyjscie z programu
				break;
			}
		}

		if (c == 27) //escape pressed
		{
			if (TheGameEngine.GameState == 1)
			{
				gameEngine::RefreshMap(); //kontunuuj gre
				return;
			}
		}

	} while (1);
}


int lastI = 0;
char buf[256];

void gameEngine::RefreshMap()
{
	//system("CLS");
	//InitGui();
	//RefreshGui();
	//ClearLog();

	if (TheGameEngine.GameState != 1)
		return;

	int x = 0;
	int y = 0;


	viewport.position.X = player.position.X - ViewportW / 2;
	viewport.position.Y = player.position.Y - ViewportH / 2;

	//InitGui();
	/*system("CLS");
	InitGui();
	RefreshGui();*/

	char c;
	mapObject tmp;
	for (y = viewport.position.Y; y < viewport.position.Y + ViewportH; y++)
	{
		for (x = viewport.position.X; x < viewport.position.X + ViewportW; x++)
		{
			tmp.position.X = x - ViewportH / 2;
			tmp.position.Y = y;



			c = TheGameEngine.area[y][x];
			if (x < 0 || x >= MapMaxX || y < 0 || y >= MapMaxY)
			{
				console::setColor(color_background);
				printf(" ");
				console::setColor(0x0F);
				continue;
			}



			switch (c)
			{
			case (char)block_player:
				console::setColor(color_player);
				break;

			case (char)(block_normal) :
				console::setColor(color_block_normal);
				break;
			case (char)(block_normal_damaged) :
				console::setColor(color_block_normal);
				break;

			case (char)block_enemy:
				console::setColor(color_block_enemy);
				break;

			case (char)block_static:
				console::setColor(color_block_static);
				break;
			}
			//if (CalculateDistance(player,tmp)<=Visibility)
			printf("%c", c);

			console::setColor(0x0F);

		}
		printf("\n");
	}
	console::setColor(color_player);
	console::putCharXY(player.position.X - viewport.position.X, player.position.Y - viewport.position.Y, block_player);


	//czy portal jest w zasiegu mapy
	tmp.position = map::GetOnScreenPos(portal.position);
	if (tmp.position.X >= 0 && tmp.position.X <= ViewportW && tmp.position.Y >= 0 && tmp.position.Y <= ViewportH)
	{
		console::setColor(color_portal);
		console::putCharXY(portal.position.X - viewport.position.X, portal.position.Y - viewport.position.Y, block_portal);
	}

	//czy sklep jest w zasiegu mapy
	tmp.position = map::GetOnScreenPos(theShop.position);
	if (tmp.position.X >= 0 && tmp.position.X <= ViewportW && tmp.position.Y >= 0 && tmp.position.Y <= ViewportH)
	{
		console::setColor(color_shop);
		console::putCharXY(theShop.position.X - viewport.position.X, theShop.position.Y - viewport.position.Y, block_shop);
	}
	console::setColor(0x0F);
}

void gameEngine::RefreshGui()
{

	if (TheGameEngine.GameState != 1)
		return;
	int hpPercent = player.hp * 15 / player.maxhp;
	int i;





	///==========HP=========

	console::setColor(0x0c);
	char c = 178;
	for (int i = 1; i <= 15; i++)
	{

		if (i > hpPercent)
		{
			console::setColor(0x08);
			c = 176;
		}

		console::putCharXY(ViewportW + 6 + i, 1, c);
	}

	console::setColor(0x0c);

	snprintf(buf, sizeof buf, "%d ", player.hp);
	console::putStrXY(ViewportW + 6 + 15 + 2, 1, buf);  //current level number

														///==========LVL=========
	int lvlPercent = player.exp * 15 / ExpToNextLevel;


	console::setColor(0x0A);
	c = 178;
	for (int i = 1; i <= 15; i++)
	{

		if (i > lvlPercent)
		{
			console::setColor(0x08);
			c = 176;
		}

		console::putCharXY(ViewportW + 6 + i, 3, c);
	}
	console::setColor(0x0A);

	snprintf(buf, sizeof buf, "%d", player.level);
	console::putStrXY(ViewportW + 7, 4, buf);  //current level number
	console::setColor(0x08);


	snprintf(buf, sizeof buf, "%d", player.level + 1);
	console::putStrXY(ViewportW + 7 + 15 - 1, 4, buf); //next level number



	console::setColor(0xD0);
	snprintf(buf, sizeof buf, "%d - %d", (int)(player.damage * MinDamageMultiplier), player.damage);
	console::putStrXY(ViewportW + 7, 6, buf); //player min and max damage number

	snprintf(buf, sizeof buf, "%d", TheGameEngine.CurrentLevel + 1);
	console::putStrXY(ViewportW + 21, 6, buf); //cave deep


											   ///==========Portal=========


	float maxDist = sqrt(pow(MapMaxX, 2) + pow(MapMaxY, 2))*0.75;
	float dist = mathem::CalculateDistance(player.position, portal.position);
	float tmp = (float)(dist / maxDist);
	float val = tmp * 12;

	console::setColor(0x08);
	c = 176;
	for (int i = 1; i <= 12; i++)
	{

		if (i > val)
		{
			console::setColor(color_portal);

			c = 178;
		}

		console::putCharXY(ViewportW + 10 + 12 - i, 8, c);
	}

	//gold number
	console::setColor(color_block_gold);
	snprintf(buf, sizeof buf, gameLang.findKey("Gold").c_str(),  player.gold);
	console::putStrXY(ViewportW + 2, 10, buf); 








}

void gameEngine::InitGui()
{
	console::setColor(0x04);
	console::putStrXY(ViewportW + 2, 1, gameLang.findKey("Hp").c_str());

	console::setColor(0x02);
	console::putStrXY(ViewportW + 2, 3, gameLang.findKey("Lvl").c_str());

	console::setColor(0x05);
	console::putStrXY(ViewportW + 2, 6, gameLang.findKey("Dmg").c_str()); console::putStrXY(ViewportW + 15, 6, gameLang.findKey("Cave").c_str());

	console::setColor(0x09);
	console::putStrXY(ViewportW + 2, 8, gameLang.findKey("Portal").c_str());
}

void gameEngine::Log(const char* text, int num)
{



	//	putStrXY(ViewportW + 7, 4, buf);  //current level number


	if (num <0)
	{
		//gracz przyjal obrazenia
		console::setColor(0x04);
		snprintf(buf, sizeof buf, "%s: %d", text, -num);
	}
	else if (num > 0)
	{
		//gracz zadal obrazenia
		console::setColor(0x0A);
		snprintf(buf, sizeof buf, "%s: %d", text, num);

	}
	else
	{
		//inna wiadomosc
		console::setColor(0x08);
		snprintf(buf, sizeof buf, "%s", text);
	}
	console::putStrXY(ViewportW + 1, ViewportH -1 - TheGameEngine.LogLevel, buf);
	TheGameEngine.LogLevel++;
}

void gameEngine::ClearLog()
{
	TheGameEngine.LogLevel = 0;
	int i = 0;
	for (i = 11; i < ViewportH; i++)
	{
		console::putStrXY(ViewportW + 0, i, "                                      ");
	}
}

void gameEngine::ShowDeathScreen()
{
	system("cls");
	console::setColor(color_logo);
	printf("                                     ____\n");
	printf("                              __,---'     `--.__\n");
	printf("                           ,-'                ; `.\n");
	printf("                          ,'                  `--.`--.\n");
	printf("                         ,'                       `._ `-.\n");
	printf("                         ;                     ;     `-- ;\n");
	printf("                       ,-'-_       _,-~~-.      ,--      `.\n");
	printf("                       ;;   `-,;    ,'~`.__    ,;;;    ;  ;\n");
	printf("                       ;;    ;,'  ,;;      `,  ;;;     `. ;\n");
	printf("                       `:   ,'    `:;     __/  `.;      ; ;\n");
	printf("                        ;~~^.   `.   `---'~~    ;;      ; ;\n");
	printf("                        `,' `.   `.            .;;;     ;'\n");
	printf("                        ,',^. `.  `._    __    `:;     ,'\n");
	printf("                        `-' `--'    ~`--'~~`--.  ~    ,'\n");
	printf("                       /;`-;_ ; ;. /. /   ; ~~`-.     ;\n");
	printf("-._                   ; ;  ; `,;`-;__;---;      `----'\n");
	printf("   `--.__             ``-`-;__;:  ;  ;__;\n");
	printf(" ...     `--.__                `-- `-'\n");
	printf("`--.:::...     `--.__                ____\n");
	printf("    `--:::::--.      `--.__    __,--'    `.\n");
	printf("        `--:::`;....       `--'       ___  `.\n");
	printf("            `--`-:::...     __           )  ;\n");
	printf("                  ~`-:::...   `---.      ( ,'\n");
	printf("                      ~`-:::::::::`--.   `-.\n");
	printf("                          ~`-::::::::`.    ;\n");
	printf("                              ~`--:::,'   ,'\n");
	printf("                                   ~~`--'~\n");


	//przezyles xxx itd.

	console::setColor(color_menu_active);
	snprintf(buf, sizeof buf, gameLang.findKey("You_Came_To").c_str(), TheGameEngine.CurrentLevel + 1);
	console::putStrXY(62, 3, buf);
	console::setColor(0xD0);
	snprintf(buf, sizeof buf, "%d", TheGameEngine.CurrentLevel + 1);
	console::putStrXY(62 + 23, 3, buf);

	console::setColor(color_menu_active);

	snprintf(buf, sizeof buf, gameLang.findKey("Reached_Level").c_str(), player.level);
	console::putStrXY(62, 5, buf);

	snprintf(buf, sizeof buf, gameLang.findKey("Killed_Mobs").c_str(), TheGameEngine.EnemiesKilled);
	console::putStrXY(62, 7, buf);

	snprintf(buf, sizeof buf, gameLang.findKey("You_Moved").c_str(), TheGameEngine.TotalTurns);
	console::putStrXY(62, 9, buf);

}

void gameEngine::ShowOptions(int initialGameState)
{
	system("cls");
	unsigned char menuIndex = 0;
	unsigned char c;


	if (TheGameEngine.GameSaveLoad)
		menuIndex = 1;
	else
		menuIndex = 0;

	int curH = 0;
	std::string langPos = "";
	int i = 1;
	for (i = 1; i <= languages.data.getLength(); i++) {
		langPos = i + 48;
		if (languages.findKey(langPos) == gameConfig.findKey("Language")) {
			langPos = tmp;
			break;
		}
	}

	do
	{
		system("cls");
		
		console::drawMenuItem(10, 10 + 6*curH, color_menu, ">");

		console::drawMenuItem(16, 10, color_menu, menuLang.findKey("Saving_System").c_str());
		if (menuIndex == 1)
			console::drawMenuItem(16 + 37, 10, 0x0A, menuLang.findKey("Yes").c_str()); //lewo
		else
			console::drawMenuItem(16 + 37, 10, 0x0A, menuLang.findKey("No").c_str()); //prawo

		console::drawMenuItem(16, 16, color_menu, menuLang.findKey("Change_Language").c_str());
		console::drawMenuItem(16 + 37, 16, 0x0A, gameConfig.findKey("Language").c_str());

		c = console::getKey();

		//do g�ry 72, w d� 80, w lewo 75, w prawo 77
		if (c == 72) {
			curH = 0;
		}
		if (c == 80) {
			curH = 1;
		}
		if (c == 75) //lewo
		{
			if (menuIndex < (2 - 1) && curH == 0)
			{
				TheGameEngine.GameSaveLoad = 1;
				menuIndex++;
			}
			else if (curH == 1) {
				i--;
				if (i <= 0) i = languages.data.getLength();
				langPos = i + 48;
				gameConfig.changeFileValue("Language", languages.findKey((langPos)), "Default");
				config::loadMainConfig();
			}
		}

		if (c == 77)//prawo
		{
			if (menuIndex >0 && curH == 0)
			{
				TheGameEngine.GameSaveLoad = 0;
				menuIndex--;
			}
			else if (curH == 1) {
				i++;
				if (i > languages.data.getLength()) i = 1;
				langPos = i + 48;
				gameConfig.changeFileValue("Language", languages.findKey((langPos)), "Default");
				config::loadMainConfig();
			}
		}

		if (c == 27) //escape pressed
		{
			TheGameEngine.GameState = initialGameState;
			system("CLS");

			return;
		}


	} while (1);
}

void gameEngine::DrawLogo()
{
	console::setColor(color_logo);


	console::putStrXY(1, 0, "_________                    ___________             .__              ");
	console::putStrXY(1, 1, "\\_   ___ \\_____ ___  __ ____ \\_   _____/__  ________ |  |   ___________  ___________ ");
	console::putStrXY(1, 2, "/    \\  \\/\\__  \\\\  \\/ // __ \\ |    __)_\\  \\/  |____ \\|  |  /  _ \\_  __ \\/ __ \\_  __ \\");
	console::putStrXY(1, 3, "\\     \\____/ __ \\\\   /\\  ___/ |        \\>    <|  |_> >  |_(  <_> )  | \\|  ___/|  | \\/");
	console::putStrXY(1, 4, " \\______  (____  /\\_/  \\___  >_______  /__/\\_ \\   __/|____/\\____/|__|   \\___  >__|   ");
	console::putStrXY(1, 5, "        \\/     \\/          \\/        \\/      \\/__|                          \\/       ");


	console::setColor(0x08);
	console::putStrXY(60, 20, "by  Jan Sudczak & Filip Strozik");
	console::setColor(0x0F);
}


