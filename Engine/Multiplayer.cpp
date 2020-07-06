#include "Multiplayer.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Console.h"
#include "UI.h"
#define BOOST_THREAD_PROVIDES_FUTURE
#include <boost/thread/future.hpp>

static const int MAX_PLAYERS = 10;

SOCKET Multiplayer::Server::Sock;
vector<shared_ptr<Multiplayer::Client>> Multiplayer::Server::Users;

shared_ptr<Multiplayer::Server> Multiplayer::Serv = make_shared<Multiplayer::Server>();
shared_ptr<Multiplayer::Client> Multiplayer::CurrentUser;
SOCKET Multiplayer::Client::Sock;

HRESULT Multiplayer::Init()
{
	::WSAStartup(MAKEWORD(2, 2), &Data);

	if (LOBYTE(Data.wVersion) != 2 || HIBYTE(Data.wVersion) != 2)
	{
		auto i = ::WSAGetLastError();
		Engine::LogError("Unknown version of WINSOCK (need 2.2)", "Unknown version of WINSOCK (need 2.2)",
			"Something is wrong with Winsock Multiplayer!");
		Destroy();
		return E_FAIL;
	}

	return S_OK;
}

void Multiplayer::Destroy()
{
	::WSACleanup();
}

#include "SDKInterface.h"
extern shared_ptr<SDKInterface> SDK;
string Log, Pas;
void Multiplayer::UpdateUI()
{
	if (!SDK->getMP()) return;
	auto DialMPConn = Application->getUI()->getDialog("MPConnection");
	auto DialMPLogin = Application->getUI()->getDialog("MPLogin");

	if (/*!DialMPLogin->getVisible() && */DialMPConn.operator bool() &&
		!DialMPConn->GetTitle().empty())
	{
		auto Comp = DialMPConn->getComponents();
		auto tbox = Comp->FindComponentChild("##MP_TextBox");
		if (Comp->FindComponentBtn("##MP_Send")->IsClicked())
		{
			tbox->getMassComponents().back()
				->FindComponentUText("##MP_Chat")->AddText(Type::Normal,
					Comp->FindComponentIText("##MP_IText")->GetText());
		}
		if (Comp->FindComponentBtn("##MP_Clear")->IsClicked())
			tbox->getMassComponents().back()
			->FindComponentUText("##MP_Chat")->ClearText();

		if (Comp->FindComponentIText("##MP_IText")->PressedEnter())
			tbox->getMassComponents().back()
			->FindComponentUText("##MP_Chat")->AddText(Type::Normal,
				Comp->FindComponentIText("##MP_IText")->GetText());

		tbox->setSize(ImVec2(450, 120));
		tbox->setAutoScroll(true);

		DialMPConn->Render();
	}

	if (!DialMPConn->getVisible() && DialMPLogin.operator bool() &&
		!DialMPLogin->GetTitle().empty())
	{
		auto Comp = DialMPLogin->getComponents();
		if (Comp->FindComponentBtn("##MP_Connect")->IsClicked() ||
			Comp->FindComponentIText("##MP_Login")->PressedEnter() ||
			Comp->FindComponentIText("##MP_Pass")->PressedEnter())
		{
			Log = Comp->FindComponentIText("##MP_Login")->GetText();
			Pas = Comp->FindComponentIText("##MP_Pass")->GetText();

			string MSG = "You connected to the server with" + Log + " and " + Pas;
			auto PrevDial = DialMPConn->getComponents();

			if (!Log.empty() && !Pas.empty() && Log == "PBAX" && Pas == "Test")
			{
				PrevDial->FindComponentChild("##MP_TextBox")->getMassComponents().back()
					->FindComponentUText("##MP_Chat")->AddText(Type::Information, MSG);

				DialMPConn->setVisible(true);
				DialMPLogin->setVisible(false);
			}
			else
			{
				MSG = "You entered wrong an User Information";
				Comp->FindComponentChild("##MP_TextBox")->getMassComponents().back()->
					FindComponentUText("##MP_Chat")->AddText(Type::Error, MSG);
			}
		}

		DialMPLogin->Render();
	}
}

#include "Camera.h"
DWORD WINAPI Multiplayer::Client::Update(LPVOID lpThreadParameter)
{
	char buffer[512];
	string Tmp;
	sockaddr_in from;
	int iSendResult = 0, bufferlen = 512, SizeOf = sizeof(from), iResult = 0;
	buffer[0] = '\0';

	while (iResult = ::recvfrom(CurrentUser->getSocket(), buffer, bufferlen, 0, (sockaddr *)&from,
		&SizeOf) != SOCKET_ERROR)
	{
		if (iResult == SOCKET_ERROR)
		{
			int error = ::WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
				Engine::LogError("shutdown failed with error: " + to_string(::WSAGetLastError()),
					string(__FILE__) + ": " + to_string(__LINE__), "shutdown failed with error: " + to_string(::WSAGetLastError()));
		}

		Tmp = buffer;

		if (contains(Tmp, "Login=true"))
		{
			vector<string> strs;
			boost::split(strs, Tmp, boost::is_any_of("|"));
			//CurrentUser->setID(atol(strs.at(1).c_str()));
		}
		//else if (contains(Tmp, "Login=false"))
		//{
		//	g_lock.lock();
		//	Engine::LogError("MP: The server denied you to log in!",
		//		"MP: The server denied you to log in!", "MP: The server denied you to log in!");
		//	g_lock.unlock();
		//}
	}
	return 0;
}
DWORD WINAPI Multiplayer::Server::Update(LPVOID lpThreadParameter)
{
	while (true)
	{
		char buffer[512];
		string Tmp;
		sockaddr_in from;
		int iSendResult = 0, bufferlen = 512, SizeOf = sizeof(from), iResult = 0;
		buffer[0] = '\0';
		while (iResult = ::recvfrom(Serv->getSock(), buffer, bufferlen, 0, (sockaddr *)&from, &SizeOf) != SOCKET_ERROR)
		{
			if (iResult == SOCKET_ERROR)
			{
				int error = ::WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
					Engine::LogError("shutdown failed with error: " + to_string(::WSAGetLastError()),
						string(__FILE__) + ": " + to_string(__LINE__), "shutdown failed with error: " + to_string(::WSAGetLastError()));
			}

			Tmp = buffer;

			vector<string> strs;
			boost::split(strs, Tmp, boost::is_any_of("="));
			USHORT slot = (USHORT)-1;

			if (Tmp == "Log in")
			{
				Users.push_back(make_shared<Client>());
				for (USHORT i = 0; i < Users.size(); ++i)
				{
					if (Users.at(i)->getAddres() == 0UL)
					{
						slot = i;
						Users.at(i)->SetAddresStruct(from);
						Users.at(i)->setID(i);
						break;
					}
				}


				if (slot != (USHORT)-1)
				{
					if (Send("Login=true|ID=" + to_string(slot), Serv->getSock(), from))
					{
						auto DialMP = Application->getUI()->getDialog("MPConnection");
						if (DialMP.operator bool())
						{
							string IP = ((boost::format("%s.%s.%s.%s") % to_string(from.sin_addr.S_un.S_un_b.s_b1)
								% to_string(from.sin_addr.S_un.S_un_b.s_b2) % to_string(from.sin_addr.S_un.S_un_b.s_b3)
								% to_string(from.sin_addr.S_un.S_un_b.s_b4)).str());
							DialMP->getComponents()->Itextmul.back().second->AddText(Type::Information,
								(boost::format("The user ID = %d, IP = %s, Port = %s.\nConnection Successful!")
									% slot %IP % to_string(from.sin_port)).str());
						}

						//client_endpoints[slot] = from_endpoint;
						//time_since_heard_from_clients[slot] = 0.0f;
						//client_objects[slot] = {};
						//client_inputs[slot] = {};
					}
				}
				else
				{
					if (Send("Login=false|ID=" + to_string(slot), Serv->getSock(), from))
					{
						//auto DialMP = Application->getUI()->getDialog("MPConnection");
						//if (DialMP.operator bool())
						//	DialMP->getComponents()->Itextmul.back()->AddCLText(Type::Error,
						//		(boost::format("The user ID = %d, IP = %s, Port = %s.\nConnection Unsuccessful!")
						//			% slot %to_string(from.sin_addr.S_un.S_addr) % to_string(from.sin_port)).str());
					}
				}
			}

			else if (contains(Tmp, "Leave")) // Leave
				Users.erase(Users.begin() + slot);
			else if (contains(Tmp, "Chat: "))
			{
				auto DialMP = Application->getUI()->getDialog("MPConnection");
				if (DialMP.operator bool())
				{
					deleteWord(Tmp, "Chat: ");
					//DialMP->getComponents()->Itextmul.back()->AddCLText(Type::Information,
					//	(string("From Some User: ") + Tmp));
				}
			}
			OutputDebugStringA((boost::format("Bytes received: %d\n") % iResult).str().c_str());
		}
	}
	return 0;
}

string Multiplayer::Receive(string msg, SOCKET Sock)
{
	char buffer[512];
	string Tmp;
	sockaddr_in from;
	int iSendResult = 0, bufferlen = 512, SizeOf = sizeof(from), iResult = 0;

	while (true)
	{
		while (iResult = ::recvfrom(Sock, buffer, bufferlen, 0, (sockaddr *)&from, &SizeOf) != SOCKET_ERROR)
		{
			if (iResult == SOCKET_ERROR)
			{
				int error = ::WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
					Engine::LogError("failed with error: " + to_string(::WSAGetLastError()),
						string(__FILE__) + ": " + to_string(__LINE__), "shutdown failed with error: " + to_string(::WSAGetLastError()));
			}

			Tmp = buffer;
			if (contains(msg, Tmp)) // "If" here are other messages
				return Tmp;
			else
				return "";
		}
	}

	return "";
}

bool Multiplayer::Send(string MSG, SOCKET sock, sockaddr_in some_addr)
{
	sockaddr_in serv_addr;
	if (some_addr.sin_addr.S_un.S_addr == 0UL)
	{
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(17000);
		serv_addr.sin_addr.S_un.S_un_b.s_b1 = 127;
		serv_addr.sin_addr.S_un.S_un_b.s_b2 = 0;
		serv_addr.sin_addr.S_un.S_un_b.s_b3 = 0;
		serv_addr.sin_addr.S_un.S_un_b.s_b4 = 1;
	}

	MSG += '\0';
	if (sendto(sock,
		MSG.c_str(), MSG.size(), 0,
		(some_addr.sin_addr.S_un.S_addr == 0) ? (sockaddr *)&serv_addr :
		(sockaddr *)&some_addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		int i = WSAGetLastError();

		Engine::LogError("MP: SendTo Is Fail!!!", "MP: SendTo Is Fail!!!",
			(boost::format("MP: Something is wrong with Client SendTo message.\nError Code: %d")
				% WSAGetLastError()).str());
		return false;
	}
	else
	{
		int i = WSAGetLastError();
		OutputDebugStringA((boost::format("\nError:%s\n") % to_string(i)).str().c_str());
		return true;
	}
	return false;
}

HRESULT Multiplayer::Connect(string IP, string Port)
{
	if (Serv.operator bool() && Serv->IsCreate())
		Serv->AddUser(make_shared<Client>());
	else
	{
		auto NewClient = make_shared<Client>();
		if (NewClient.operator bool() && NewClient->Connect())
			CurrentUser = NewClient;
		else
			return E_FAIL;
	}
	return S_OK;
}

bool Multiplayer::Client::Connect()
{
	if (getCurrentUser().operator bool() && getCurrentUser()->getAddres() != 0UL)
	{
		//auto DialMP = Application->getUI()->getDialog("MPConnection");
		//if (DialMP.operator bool())
		//	DialMP->getComponents()->Itextmul.back()->AddCLText(Type::Information, string("From Server:\n"\
		//		"You're connected in this moment"));
		return false;
	}

	Sock = ::socket(AF_INET, SOCK_STREAM, NULL);
	if (Sock == INVALID_SOCKET)
	{
		int i = WSAGetLastError();

		Engine::LogError("accept failed with error: " + to_string(::WSAGetLastError()),
			string(__FILE__) + ": " + to_string(__LINE__), "accept failed with error: " + to_string(::WSAGetLastError()));
		::closesocket(Sock);
		Destroy();
		return false;
	}

	//	Send("Log in", Sock);
	return true;
}

void Multiplayer::Server::Create()
{
	if (Sock != 0)
	{
		//auto DialMP = Application->getUI()->getDialog("MPConnection");
		//if (DialMP.operator bool())
		//	DialMP->getComponents()->Itextmul.back()->AddCLText(Type::Error, string("System:\n"\
		//		"The server already created!"));
		return;
	}

	int iResult;

	Sock = ::socket(AF_INET, SOCK_STREAM, NULL);
	if (Sock == INVALID_SOCKET)
	{
		int i = WSAGetLastError();
		::closesocket(Sock);

		Engine::LogError("socket failed with error: " + to_string(::WSAGetLastError()),
			string(__FILE__) + ": " + to_string(__LINE__), "socket failed with error: " + to_string(::WSAGetLastError()));
		Destroy();
	}

	ZeroMemory(&Server, sizeof(Server));
	Server.sin_family = AF_INET;
	Server.sin_addr.s_addr = INADDR_ANY;
	Server.sin_port = htons(17000);

	iResult = ::bind(Sock, (sockaddr *)&Server, sizeof(Server));
	if (iResult == SOCKET_ERROR)
	{
		int i = WSAGetLastError();

		Engine::LogError("bind failed with error: " + to_string(::WSAGetLastError()),
			string(__FILE__) + ": " + to_string(__LINE__), "bind failed with error: " + to_string(::WSAGetLastError()));
		::closesocket(Sock);
		Destroy();
	}

	u_long enabled = 1;
	ioctlsocket(Sock, FIONBIO, &enabled);

	iResult = ::WSAGetLastError();

	IsCreated = true;
	CreateThread(NULL, 0, Multiplayer::Server::Update, NULL, 0, NULL);
}

void Multiplayer::Client::Disconnect()
{
	Send("Leave ID=" + to_string(ID), Sock, clientAddr);
	auto iResult = shutdown(Sock, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		int i = WSAGetLastError();

		Engine::LogError("shutdown failed with error: " + to_string(::WSAGetLastError()),
			string(__FILE__) + ": " + to_string(__LINE__), "shutdown failed with error: " + to_string(::WSAGetLastError()));
		::closesocket(Sock);
		Destroy();
	}

	::closesocket(Sock);
}

void Multiplayer::Server::AddUser(shared_ptr<Multiplayer::Client> usr)
{
	if (Users.empty() || Users.size() <= MAX_PLAYERS)
	{
		auto NewClient = make_shared<Client>();
		if (NewClient.operator bool() && NewClient->Connect())
			CurrentUser = NewClient;
		else
			Engine::LogError("MP: Connection established",
				"MP: Connection established", "MP: Connection established or You're already connected.\n"\
				"Please, try again later)");
		CreateThread(NULL, 0, Multiplayer::Client::Update, NULL, 0, NULL);
	}
	else
		Engine::LogError("MP: Server is full",
			"MP: Server is full", "MP: The server has maximum players now. Please, try later)");
}
