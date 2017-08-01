#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <random>

using namespace std;

enum eRES { eRES_OK=0, eRES_ERR_UPLOAD_IMAGE, eRES_ERR_UPLOAD_FONT};
enum eSE {eSE_NONE=0, eSE_EARTHQUAKE, eSE_MINIMAL, eSE_FASTTRACK, eSE_TOTAL};

class CEngine
{
public:
	// Creation
	CEngine();
	~CEngine();
	eRES CreateElements();
	eRES CreateMainWin();
	eRES CreateStartBtn();
	eRES CreateBtn1();
	eRES CreateTextScore();
	eRES CreateTextGlobalTime();
	eRES CreateProgBar();
	void DrawElements();
	// Events
	void OnExpiryBtn1();
	void OnClickStartBtn();
	void OnClickBtn1();
	void OnEventEarthquake();
	void OnEventFastTrack();
	void OnEventMinimal();
	// Functionality
	eRES RunGameLoop();
	void HandleEvents();
	void HandleSpecialEvents();
	void CheckTriggerSE();
	void UpdateGlobalTime();
	void UpdateProgressBar();
	void PaintBackground(sf::Color sfclrInput);
	void ReposBtn1();
	void ColorBtn1();
	void CheckExpiryBt1();
	void ShrinkBtn1();
	bool IsAreaClicked(float fClickX, float fClickY, float fAreaX, float fAreaY, float fAreaSizeX, float fAreaSizeY);
	
private:
	sf::Clock			m_sfclcBtnExpiry;
	sf::Clock			m_sfclcGlobal;
	sf::Clock			m_sfclcEvent;
	sf::RenderWindow	m_sfwinMain;
	sf::Sprite			m_sfsprBackground;
	sf::Texture			m_sftxBackground;
	sf::Sprite			m_sfsprStartBtn;
	sf::Texture			m_sftxStartBtn;
	sf::RectangleShape	m_sfrectBtn1;
	sf::Texture			m_sftxBtn1;
	sf::Text			m_sftxtScore;
	sf::Font			m_sffntScore;
	sf::Text			m_sftxtGlobalTime;
	sf::Sprite			m_sfsprProgBar;
	sf::Texture			m_sftxProgBar;
	int					m_nScore;
	bool				m_bShowStartBtn;
	float				m_fGlobalTime;
	float				m_fDurationBtn1;
	eSE					m_nActiveSE;
	float				m_fBtn1Length;
	float				m_fBtn1Height;
};