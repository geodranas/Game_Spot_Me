#include "Engine.h"

//// Game Parameters ////
const int nGAME_WIDTH = 800;
const int nGAME_HEIGHT = 600;
const int nMAX_GLOBAL_TIME = 100; //// sec
const int nPROGR_BAR_TOP_POINT = 30;
const int nTXT_GLOBAL_TIME_TOP_POINT = 10;
const int nPROGR_BAR_HEIGHT = 20;
const int nTIME_BONUS_BTN1 = 2; ////sec	
const float fTXT_SCORE_POS_FACTOR = (float)0.9;  //// percent of total height in the screen where Score will appear
const float fBTN1_LENGTH = (float)50;
const float fBTN1_HEIGHT = (float)50;
const float fBTN1_OUT_THICKNESS = (float)2;
const int nEARTHQ_RANGE = 5;			//// positional variance of windown in the
const int nEARTHQ_VAR = 2;					// event of an earthquake
const int nSE_PROB_SPACE = 10000;	//// possibility of a  			
const int nSE_PROB_TRIGGER = 20;		// special event (SE) occuring
const int nSE_DURATION = 15;			////sec
const float fLIFETIME_BTN1 = (float)1;	////sec
sf::Color sfclrBACKGRND_DEFAULT = sf::Color(200, 200, 200);		//// grey
sf::Color sfclrBACKGRND_FASTTRACK = sf::Color(100, 0, 100); //// dark pink
const float fFASTTRACK_SPEED_FACTOR = 0.80;			//// fasttrack event decrease button1 lifetime

CEngine::CEngine()
	:m_sfwinMain(sf::VideoMode(nGAME_WIDTH, nGAME_HEIGHT, 32), "Spot Me", sf::Style::Titlebar | sf::Style::Close)
{
	srand(static_cast<unsigned int>(std::time(NULL))); // Init Random Machine
	m_nScore = 0;
	m_bShowStartBtn = true;
	m_fGlobalTime = (float) nMAX_GLOBAL_TIME;
	m_nActiveSE = eSE_NONE;
	m_fDurationBtn1 = fLIFETIME_BTN1;
	m_fBtn1Height = fBTN1_HEIGHT;
	m_fBtn1Length = fBTN1_LENGTH;
}

CEngine::~CEngine()
{
}

eRES CEngine::RunGameLoop()
{
	// Create Basic Elements
	eRES eResult = CreateElements();

	if (eResult != eRES_OK)
		return eResult;

	// Main App Handler
	while (m_sfwinMain.isOpen())
	{
		// Clear Main Window
		m_sfwinMain.clear();

		// Draw Background
		m_sfwinMain.draw(m_sfsprBackground);

		// Draw Basic Elements
		if (m_bShowStartBtn) 
			m_sfwinMain.draw(m_sfsprStartBtn);
		else 				
			DrawElements();

		// Update Global Time
		UpdateGlobalTime();
		
		// Include Special Events
		HandleSpecialEvents();
		
		// Event Handling
		HandleEvents();

		// Update Elements
		CheckExpiryBt1();
		ColorBtn1();
		UpdateProgressBar();
		
		// Display total window
		m_sfwinMain.display();
	}

	return eRES_OK;
}

void CEngine::HandleSpecialEvents()
{
	switch (m_nActiveSE)
	{
		case (eSE_NONE) :
		{
			CheckTriggerSE();
			break;
		}
		case (eSE_EARTHQUAKE):
		{
			OnEventEarthquake();
			break;
		}
		case (eSE_FASTTRACK) :
		{
			OnEventFastTrack();
			break;
		}
		case (eSE_MINIMAL):
		{
			OnEventMinimal();
			break;
		}
	}
}

void CEngine::CheckTriggerSE()
{
	int nRandNum = rand() % nSE_PROB_SPACE;

	// Check if a special event will be activated
	if (nSE_PROB_TRIGGER > nRandNum)
	{
		// Randomly choose special event from list
		m_nActiveSE= static_cast<eSE>( rand() % (eSE_TOTAL - 1)+1);

		// Reset Event Clock
		m_sfclcEvent.restart();
	}
}

void CEngine::OnEventEarthquake()
{
	// Check if event has expired
	if (m_sfclcEvent.getElapsedTime().asSeconds() > nSE_DURATION)
	{	
		// Kill Event
		m_nActiveSE = eSE_NONE;
		PaintBackground(sfclrBACKGRND_DEFAULT);
	}
	else
	{
		// Run Special Event Earthquake

		// Introduce Lightning Effect
		if (((int)m_sfclcEvent.getElapsedTime().asMicroseconds() %4)==0)
			PaintBackground(sf::Color::Black);
		else
			PaintBackground(sf::Color::White);

		// Move Window in an earthquaky look
		int nPosX = m_sfwinMain.getPosition().x + rand() % nEARTHQ_RANGE - nEARTHQ_VAR;
		int nPosY = m_sfwinMain.getPosition().y + rand() % nEARTHQ_RANGE - nEARTHQ_VAR;

		m_sfwinMain.setPosition(sf::Vector2i(nPosX, nPosY));
	}
}

void CEngine::OnEventFastTrack()
{
	// Check if event has expired
	if (m_sfclcEvent.getElapsedTime().asSeconds() > nSE_DURATION)
	{
		// Kill Event
		m_nActiveSE = eSE_NONE;

		// Reset Affected Elements
		PaintBackground(sfclrBACKGRND_DEFAULT); 
		m_fDurationBtn1 = fLIFETIME_BTN1;
	}
	else
	{
		// Run Special Event FastTrack

		// Apply FastTrack Background Color
		PaintBackground(sfclrBACKGRND_FASTTRACK);

		// Decrease the lifetime of Button1 (temporarily)
		m_fDurationBtn1 = fLIFETIME_BTN1*fFASTTRACK_SPEED_FACTOR;
	}
}

void CEngine::OnEventMinimal()
{
	// Check if event has expired
	if (m_sfclcEvent.getElapsedTime().asSeconds() > nSE_DURATION)
	{
		// Kill Event
		m_nActiveSE = eSE_NONE;
		PaintBackground(sfclrBACKGRND_DEFAULT);
		m_fBtn1Height = fBTN1_HEIGHT;
		m_fBtn1Length = fBTN1_LENGTH;
		m_sfrectBtn1.setSize(sf::Vector2f(m_fBtn1Length, m_fBtn1Height));
	}
	else
	{
		// Run Special Event Minimal

		// Change Background
		PaintBackground(sf::Color::Cyan);

		// Make Smaller Buttons
		ShrinkBtn1();
	}
}

void CEngine::PaintBackground(sf::Color sfclrInput)
{
	sf::Image sfimgBackground;
	sfimgBackground.create(nGAME_WIDTH, nGAME_HEIGHT, sfclrInput);	// Gray Background

	m_sftxBackground.loadFromImage(sfimgBackground);
	m_sfsprBackground.setTexture(m_sftxBackground, true);
}

void CEngine::UpdateProgressBar()
{
	// Set Progress Bar position
	int nNewLength = (int)((m_fGlobalTime * nGAME_HEIGHT) / nMAX_GLOBAL_TIME);
	m_sfsprProgBar.setTextureRect(sf::IntRect(0, nPROGR_BAR_TOP_POINT, nNewLength, nPROGR_BAR_HEIGHT));
}

void CEngine::OnClickStartBtn()
{
	// Flag so that start btn only appers once
	m_bShowStartBtn = false;

	// Start all elements of Main Game
	ReposBtn1();
	m_sfclcBtnExpiry.restart();
	m_sfclcGlobal.restart();
}

void CEngine::OnClickBtn1()
{
	// Reposition Button 1
	ReposBtn1();
	m_sfclcBtnExpiry.restart();

	// Update Score
	m_nScore++;
	m_sftxtScore.setString("Score: " + to_string(m_nScore));

	// Update Global Time (add time bonus at successful click of button1)
	if (m_fGlobalTime + nTIME_BONUS_BTN1 > nMAX_GLOBAL_TIME)
		m_fGlobalTime = nMAX_GLOBAL_TIME;
	else
		m_fGlobalTime += nTIME_BONUS_BTN1;
}

void CEngine::OnExpiryBtn1()
{
	// When user does not click in time
	ReposBtn1();
	m_sfclcBtnExpiry.restart();
}

void CEngine::HandleEvents()
{
	sf::Event sf_evnMainWin;

	while (m_sfwinMain.pollEvent(sf_evnMainWin))
	{
		switch (sf_evnMainWin.type)
		{
			case sf::Event::Closed:
			{
				m_sfwinMain.close();
				break;
			}
			case sf::Event::MouseButtonPressed:
			{
				// Calculate Click position
				float sfposClickX = (float)sf::Mouse::getPosition(m_sfwinMain).x;
				float sfposClickY = (float)sf::Mouse::getPosition(m_sfwinMain).y;

				if (m_bShowStartBtn)	// only when entering the game for the first time
				{
					// Calculate Position of Start Button
					sf::Vector2f sfposStartBtn;
					sfposStartBtn.x = m_sfsprStartBtn.getPosition().x - (float)m_sftxStartBtn.getSize().x / 2;
					sfposStartBtn.y = m_sfsprStartBtn.getPosition().y - (float)m_sftxStartBtn.getSize().y / 2;

					if (IsAreaClicked(sfposClickX, sfposClickY, sfposStartBtn.x, sfposStartBtn.y, (float)m_sftxStartBtn.getSize().x, (float)m_sftxStartBtn.getSize().y))
						OnClickStartBtn();
				}
				else				// main game mode
				{
					if (IsAreaClicked(sfposClickX, sfposClickY, m_sfrectBtn1.getPosition().x, m_sfrectBtn1.getPosition().y, m_sfrectBtn1.getSize().x, m_sfrectBtn1.getSize().y))
						OnClickBtn1();
				}
				break;
			}
			case sf::Event::KeyPressed:	
			{
				if (sf_evnMainWin.key.code == sf::Keyboard::Escape)
					m_sfwinMain.close();
				else
					OnClickBtn1(); 	////////for testing only SO AS NOT TO KEEP ON CLICKING!
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void CEngine::DrawElements()
{
	// Draw Basic Board Elements
	m_sfwinMain.draw(m_sfrectBtn1);
	m_sfwinMain.draw(m_sftxtScore);
	m_sfwinMain.draw(m_sftxtGlobalTime);
	m_sfwinMain.draw(m_sfsprProgBar);
}

eRES CEngine::CreateElements()
{
	// Create App Window
	eRES eResult = CreateMainWin();

	if (eResult != eRES_OK)
		return eResult;

	// Create Start Button
	eResult = CreateStartBtn();

	if (eResult != eRES_OK)
		return eResult;

	// Create Button 1
	eResult = CreateBtn1();

	if (eResult != eRES_OK)
		return eResult;

	// Create Text Score
	eResult=CreateTextScore();

	if (eResult != eRES_OK)
		return eResult;
	else
		CreateTextGlobalTime();

	// Create Progress Time Bar
	eResult = CreateProgBar();

	if (eResult != eRES_OK)
		return eResult;

	return eRES_OK;
}

eRES CEngine::CreateMainWin()
{
	// Added to avoid de-sync
	m_sfwinMain.setVerticalSyncEnabled(true);
	
	// Use background
	PaintBackground(sfclrBACKGRND_DEFAULT);

	return eRES_OK;
}

eRES CEngine::CreateStartBtn()
{
	// Starting Screen Button with game rules
	sf::Image sfimgStart;

	if (!sfimgStart.loadFromFile("res/Start.png"))
		return eRES_ERR_UPLOAD_IMAGE;

	m_sftxStartBtn.loadFromImage(sfimgStart);
	m_sfsprStartBtn.setTexture(m_sftxStartBtn, true);
	m_sfsprStartBtn.setOrigin((float)m_sftxStartBtn.getSize().x / 2, (float)m_sftxStartBtn.getSize().y / 2);
	m_sfsprStartBtn.setPosition((float)nGAME_WIDTH / 2, (float)nGAME_HEIGHT / 2);

	return eRES_OK;
}

eRES CEngine::CreateBtn1()
{
	// Main Game button creation
	m_sfrectBtn1.setSize(sf::Vector2f(m_fBtn1Length, m_fBtn1Height));
	m_sfrectBtn1.setOutlineThickness(fBTN1_OUT_THICKNESS);
	m_sfrectBtn1.setOutlineColor(sf::Color::Black);
	
	return eRES_OK;
}

eRES CEngine::CreateTextScore()
{
	// Create Text Score
	if (!m_sffntScore.loadFromFile("arial_narrow_7.ttf"))
		return eRES_ERR_UPLOAD_FONT;

	m_sftxtScore.setString("Score: 0");
	m_sftxtScore.setFont(m_sffntScore);
	m_sftxtScore.setCharacterSize(20);
	m_sftxtScore.setStyle(sf::Text::Regular);
	m_sftxtScore.setColor(sf::Color::Red);
	m_sftxtScore.setOrigin((float)m_sftxtScore.getGlobalBounds().width / 2, (float)m_sftxtScore.getGlobalBounds().height / 2);
	m_sftxtScore.setPosition(sf::Vector2f((float)nGAME_WIDTH / 2, (float)fTXT_SCORE_POS_FACTOR*nGAME_HEIGHT));

	return eRES_OK;
}

eRES CEngine::CreateTextGlobalTime()
{
	// Create Text Global Time
	m_sftxtGlobalTime.setString("Time Left");
	m_sftxtGlobalTime.setFont(m_sffntScore);
	m_sftxtGlobalTime.setCharacterSize(20);
	m_sftxtGlobalTime.setStyle(sf::Text::Regular);
	m_sftxtGlobalTime.setColor(sf::Color::Red);
	m_sftxtGlobalTime.setOrigin((float)m_sftxtGlobalTime.getGlobalBounds().width / 2, (float)m_sftxtGlobalTime.getGlobalBounds().height / 2);
	m_sftxtGlobalTime.setPosition(sf::Vector2f((float)nGAME_WIDTH / 2, (float)nTXT_GLOBAL_TIME_TOP_POINT));

	return eRES_OK;
}

eRES CEngine::CreateProgBar()
{
	// Creation of Progress (Time)Bar
	sf::Image sfimgProgBar;

	if (!sfimgProgBar.loadFromFile("res/Bar.png"))     
		return eRES_ERR_UPLOAD_IMAGE;

	m_sftxProgBar.loadFromImage(sfimgProgBar);
	m_sfsprProgBar.setTexture(m_sftxProgBar, true);
	m_sfsprProgBar.setPosition((float)nGAME_WIDTH/2- m_sftxProgBar.getSize().x/2, (float)nPROGR_BAR_TOP_POINT);
	
	return eRES_OK;
}

void CEngine::UpdateGlobalTime()
{
	m_fGlobalTime -= m_sfclcGlobal.getElapsedTime().asSeconds();
	m_sfclcGlobal.restart();
}

void CEngine::CheckExpiryBt1()
{
	//Check if Btn1 needs to be repositioned due to time expiry
	float sElapsedTime = m_sfclcBtnExpiry.getElapsedTime().asSeconds();

	if (sElapsedTime > m_fDurationBtn1) 
		OnExpiryBtn1();
}

void CEngine::ReposBtn1()
{
	// Check that when repositioned, button1 won't exceed the borders or overlap with other board elements
	int nProgBarAreaHeight= nPROGR_BAR_TOP_POINT + nPROGR_BAR_HEIGHT;

	float fNewPosX = (float)(rand() % (int)(nGAME_WIDTH - m_sfrectBtn1.getSize().x- fBTN1_OUT_THICKNESS));
	float fNewPosY = (float)(rand() % (int)(nGAME_HEIGHT - m_sfrectBtn1.getSize().y- fBTN1_OUT_THICKNESS - nProgBarAreaHeight)+ nProgBarAreaHeight);
	
	m_sfrectBtn1.setPosition(sf::Vector2f(fNewPosX, fNewPosY));
}

void CEngine::ShrinkBtn1()
{
	float sElapsedTime = m_sfclcBtnExpiry.getElapsedTime().asSeconds();
	float fSizeLeftFactor = (float)1 - (sElapsedTime / m_fDurationBtn1);
	m_fBtn1Height = fSizeLeftFactor* fBTN1_LENGTH;
	m_fBtn1Length = fSizeLeftFactor*fBTN1_HEIGHT;

	m_sfrectBtn1.setSize(sf::Vector2f(m_fBtn1Length, m_fBtn1Height));
}

void CEngine::ColorBtn1()
{
	float sElapsedTime = m_sfclcBtnExpiry.getElapsedTime().asSeconds();

	int nRed, nGreen;
	float vfTimeLevel[5];

	// Split lifetime of button 1 into five equal color periods ---> cosmetic reasons only
	for (unsigned int i = 0; i < 5; i++)
		vfTimeLevel[i] = (float)(m_fDurationBtn1*0.2*(i + 1));

	if (sElapsedTime <= vfTimeLevel[0])
	{
		nRed = (int)0;
		nGreen =(int) (120 * (1 + sElapsedTime / vfTimeLevel[0]));
	}
	else if (sElapsedTime > vfTimeLevel[0] && sElapsedTime <= vfTimeLevel[1])
	{
		nRed = (int) (120*(sElapsedTime-vfTimeLevel[0])/ vfTimeLevel[0]);
		nGreen = (int)240;
	}
	else if (sElapsedTime > vfTimeLevel[1] && sElapsedTime <= vfTimeLevel[2])
	{
		nRed = 	(int)(120 + 120 * ((sElapsedTime - vfTimeLevel[1]) / vfTimeLevel[0]));
		nGreen = (int)240;
	}
	else if (sElapsedTime > vfTimeLevel[2] && sElapsedTime <= vfTimeLevel[3])
	{
		nRed = (int)240;
		nGreen = (int)(240 - 120 * ((sElapsedTime - vfTimeLevel[2]) / vfTimeLevel[0]));
	}
	else if (sElapsedTime > vfTimeLevel[3] && sElapsedTime <= vfTimeLevel[4])
	{
		nRed = (int)240;
		nGreen = (int)(120 - 120 * ((sElapsedTime - vfTimeLevel[3]) / vfTimeLevel[0]));
	}

	m_sfrectBtn1.setFillColor(sf::Color(nRed, nGreen, 0));
}

bool CEngine::IsAreaClicked(float fClickX, float fClickY, float fAreaX, float fAreaY, float fAreaSizeX, float fAreaSizeY)
{
	return	(fClickX >= fAreaX && fClickX < fAreaX + fAreaSizeX && fClickY >= fAreaY && fClickY < fAreaY + fAreaSizeY);
}