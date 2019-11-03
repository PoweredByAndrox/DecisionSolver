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

static vector<shared_ptr<Multiplayer::Cobes>> Cobe;

#include <thread>
#include <WS2tcpip.h>
#include <mutex>
mutex g_lock;

void Multiplayer::MatToStr(Matrix Thing, string & str)
{
	str += to_string(Thing._11);
	str += "," + to_string(Thing._12);
	str += "," + to_string(Thing._13);
	str += "," + to_string(Thing._14);

	str += "," + to_string(Thing._21);
	str += "," + to_string(Thing._22);
	str += "," + to_string(Thing._23);
	str += "," + to_string(Thing._24);

	str += "," + to_string(Thing._31);
	str += "," + to_string(Thing._32);
	str += "," + to_string(Thing._33);
	str += "," + to_string(Thing._34);

	str += "," + to_string(Thing._41);
	str += "," + to_string(Thing._42);
	str += "," + to_string(Thing._43);
	str += "," + to_string(Thing._44);
}

void Multiplayer::StrToMat(Matrix & Thing, string str)
{
	vector<string> mat;
	boost::split(mat, str, boost::is_any_of(","));
	Thing._11 = (float)atof(mat.at(0).c_str());
	Thing._12 = (float)atof(mat.at(1).c_str());
	Thing._13 = (float)atof(mat.at(2).c_str());
	Thing._14 = (float)atof(mat.at(3).c_str());

	Thing._21 = (float)atof(mat.at(4).c_str());
	Thing._22 = (float)atof(mat.at(5).c_str());
	Thing._23 = (float)atof(mat.at(6).c_str());
	Thing._24 = (float)atof(mat.at(7).c_str());

	Thing._31 = (float)atof(mat.at(8).c_str());
	Thing._32 = (float)atof(mat.at(9).c_str());
	Thing._33 = (float)atof(mat.at(10).c_str());
	Thing._34 = (float)atof(mat.at(11).c_str());

	Thing._41 = (float)atof(mat.at(12).c_str());
	Thing._42 = (float)atof(mat.at(13).c_str());
	Thing._43 = (float)atof(mat.at(14).c_str());
	Thing._44 = (float)atof(mat.at(15).c_str());
}

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

void Multiplayer::Update()
{
	//Multiplayer::Client::Update();
	for (size_t i = 0; i < Cobe.size(); i++)
	{
		Cobe.at(i)->Render();
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
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
				throw exception("HERE!!!");
		}

		Tmp = buffer;
		auto ObjCam = Application->getCamera();

		if (contains(Tmp, "Login=true"))
		{
			vector<string> strs;
			boost::split(strs, Tmp, boost::is_any_of("|"));
			CurrentUser->setID(atol(strs.at(1).c_str()));
		}
		//else if (contains(Tmp, "Login=false"))
		//{
		//	g_lock.lock();
		//	Engine::LogError("MP: The server denied you to log in!",
		//		"MP: The server denied you to log in!", "MP: The server denied you to log in!");
		//	g_lock.unlock();
		//}

		else if (contains(Tmp, "|") && !Cobe.empty() && ObjCam.operator bool())
		{
			Vector3 tmpP = Vector3::Zero, tmpR = Vector3::Zero;
			vector<float> Pos, Rot;
			vector<string> strs;

			boost::split(strs, Tmp, boost::is_any_of("|"));

			getFloat3Text(strs.at(1), ",", Pos);
			getFloat3Text(strs.at(2), ",", Rot);

			tmpP.x = Pos.at(0);
			tmpP.y = Pos.at(1);
			tmpP.z = Pos.at(2);

			tmpR.x = Rot.at(0);
			tmpR.y = Rot.at(1);
			tmpR.z = Rot.at(2);

			Cobe.at(atoi(strs.at(0).c_str()))->Pos = tmpP;
			Cobe.at(atoi(strs.at(0).c_str()))->Rotate = tmpR;

			string Matrixes = to_string(CurrentUser->getID()) + "|";
			MatToStr(ObjCam->GetViewMatrix(), Matrixes);
			Matrixes += '|';
			MatToStr(ObjCam->GetProjMatrix(), Matrixes);
			Send(Matrixes, CurrentUser->getSocket());
		}
	}
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
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
					throw exception("HERE!!!");
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
						g_lock.lock();
						//auto DialMP = Application->getUI()->getDialog("MPConnection");
						//if (DialMP.operator bool())
						//{
						//	string IP = ((boost::format("%s.%s.%s.%s") % to_string(from.sin_addr.S_un.S_un_b.s_b1)
						//		% to_string(from.sin_addr.S_un.S_un_b.s_b2) % to_string(from.sin_addr.S_un.S_un_b.s_b3)
						//		% to_string(from.sin_addr.S_un.S_un_b.s_b4)).str());
						//	DialMP->getComponents()->Itextmul.back()->AddCLText(Type::Information,
						//		(boost::format("The user ID = %d, IP = %s, Port = %s.\nConnection Successful!")
						//			% slot %IP % to_string(from.sin_port)).str());
						//}
						Cobe.push_back(make_shared<Cobes>());

						g_lock.unlock();

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
						g_lock.lock();
						//auto DialMP = Application->getUI()->getDialog("MPConnection");
						//if (DialMP.operator bool())
						//	DialMP->getComponents()->Itextmul.back()->AddCLText(Type::Error,
						//		(boost::format("The user ID = %d, IP = %s, Port = %s.\nConnection Unsuccessful!")
						//			% slot %to_string(from.sin_addr.S_un.S_addr) % to_string(from.sin_port)).str());
						g_lock.unlock();
					}
				}
			}

			else if (contains(Tmp, "Leave")) // Leave
				Users.erase(Users.begin() + slot);
			else if (contains(Tmp, "Chat: "))
			{
				g_lock.lock();
				auto DialMP = Application->getUI()->getDialog("MPConnection");
				if (DialMP.operator bool())
				{
					deleteWord(Tmp, "Chat: ");
					//DialMP->getComponents()->Itextmul.back()->AddCLText(Type::Information,
					//	(string("From Some User: ") + Tmp));
				}
				g_lock.unlock();
			}
			else if (contains(Tmp, "|"))
			{
				boost::split(strs, Tmp, boost::is_any_of("|"));
				Matrix View, Proj;
				StrToMat(View, strs.at(1));
				StrToMat(Proj, strs.at(2));
				Cobe.at(atoi(strs.at(0).c_str()))->View = View;
				Cobe.at(atoi(strs.at(0).c_str()))->Proj = Proj;
			}
			OutputDebugStringA((boost::format("Bytes received: %d\n") % iResult).str().c_str());
		}

		if (!Cobe.empty())
			for (size_t i = 0; i < Users.size(); i++)
			{
				string msg = to_string(i) + '|';
				Vector3 tmpP = Cobe.at(i)->Pos, tmpR = Cobe.at(i)->Rotate;
				getTextFloat3(msg, ",", vector<float> {tmpP.x, tmpP.y, tmpP.z});
				msg += '|';
				getTextFloat3(msg, ",", vector<float> {tmpR.x, tmpR.y, tmpR.z});

				Send(msg, Serv->getSock(), Users.at(i)->getAddresStruct());
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
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
					throw exception("HERE!!!");
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
		throw exception("HERE!!!");

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

	Sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Sock == INVALID_SOCKET)
	{
		int i = WSAGetLastError();

		//printf("accept failed with error: %d\n", ::WSAGetLastError());
		::closesocket(Sock);
		Destroy();
		throw exception("HERE!!!");
		return false;
	}

	Send("Log in", Sock);
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

	Sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Sock == INVALID_SOCKET)
	{
		int i = WSAGetLastError();
		::closesocket(Sock);

		printf("socket failed with error: %ld\n", ::WSAGetLastError());
		Destroy();
		throw exception("HERE!!!");
	}

	ZeroMemory(&Server, sizeof(Server));
	Server.sin_family = AF_INET;
	Server.sin_addr.s_addr = INADDR_ANY;
	Server.sin_port = htons(17000);

	iResult = ::bind(Sock, (sockaddr *)&Server, sizeof(Server));
	if (iResult == SOCKET_ERROR)
	{
		int i = WSAGetLastError();

		printf("bind failed with error: %d\n", ::WSAGetLastError());
		::closesocket(Sock);
		Destroy();
		throw exception("HERE!!!");
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

		printf("shutdown failed with error: %d\n", ::WSAGetLastError());
		::closesocket(Sock);
		Destroy();
		throw exception("HERE!!!");
	}

	::closesocket(Sock);
}

void Multiplayer::Server::AddUser(shared_ptr<Multiplayer::Client> usr)
{
	if (Users.empty() || Users.size() < MAX_PLAYERS)
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

void Multiplayer::Cobes::Render()
{
	auto P = GeometricPrimitive::CreateCube(Application->getDeviceContext(), 5.f, false);
	auto objCam = Application->getCamera();
	P->Draw(/*Matrix::CreateTranslation(Rotate) **/ Matrix::CreateTranslation(View.Translation()),
		View, Proj, Colors::DarkSeaGreen, nullptr, Application->IsWireFrame());
}
