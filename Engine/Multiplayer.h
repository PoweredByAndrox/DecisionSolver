#pragma once
#if !defined(__MULTIPLAYER_H__)
#define __MULTIPLAYER_H__

#include <WinSock2.h>
#include "pch.h"

#include "StepTimer.h"
class Multiplayer
{
public:
	struct Cobes
	{
		Vector3 Pos = Vector3::Zero, Rotate = Vector3::Zero;
		Matrix View, Proj;
		void Render();
	};

	static void MatToStr(Matrix Thing, string &str);
	static void StrToMat(Matrix &Thing, string str);

	class Server;
	class Client
	{
	public:
		bool Connect();
		void Disconnect();
		static DWORD WINAPI Update(LPVOID lpThreadParameter);

		sockaddr_in getAddresStruct() { return clientAddr; }
		SOCKET getSocket() { return Sock; }
		ULONG getAddres() { return clientAddr.sin_addr.S_un.S_addr; }
		USHORT getPort() { return clientAddr.sin_port; }
		auto getTimer() { return tmr; }
		USHORT getID() { return ID; }

		void SetAddresStruct(sockaddr_in StructAddr) { clientAddr = StructAddr; }
		void SetSocket(SOCKET s) { Sock = s; }
		void setID(USHORT id) { ID = id; }
	private:
		static SOCKET Sock;
		sockaddr_in clientAddr;
		UINT address = 0U;
		USHORT ID = -1;
		shared_ptr<StepTimer> tmr = make_shared<StepTimer>();
	};
	class Server
	{
	public:
		void Create();
		static DWORD WINAPI Update(LPVOID lpThreadParameter);
		void AddUser(shared_ptr<Multiplayer::Client> usr);

		auto getUsers() { return Users; }
		auto getSock() { return Sock; }
		auto getServer() { return Server; }
		bool IsCreate() { return IsCreated; }
	private:
		sockaddr_in Server;
		static SOCKET Sock;
		static vector<shared_ptr<Multiplayer::Client>> Users;

		bool IsCreated = false;
	};

	HRESULT Init();
	static void Destroy();
	void Update();

	static string Receive(string msg, SOCKET Sock);
	static bool Send(string MSG, SOCKET sock, sockaddr_in some_addr = sockaddr_in());
	HRESULT Connect(string IP = "decisionsolver.hopto.org", string Port = "17000");

	WSADATA getData() { return Data; }
	shared_ptr<Multiplayer::Server> getServer() { return Serv; }
	static shared_ptr<Multiplayer::Client> getCurrentUser() { return CurrentUser; }
private:
	WSADATA Data;
	static shared_ptr<Multiplayer::Server> Serv;
	static shared_ptr<Multiplayer::Client> CurrentUser;
};
#endif // !__MULTIPLAYER_H__
