#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include "server.hpp"
#include <unistd.h>
#include <cstring>
#include<vector>
#include <sys/poll.h>

void accept_new_connection(int server_socket, std::vector<pollfd>& poll_fds, struct sockaddr_in socketAddress, socklen_t socketAdressLength)
{
	(void)poll_fds;
	char msg_to_send[BUFFER_SIZE];
    int connectedSocketFD = accept(server_socket,(struct sockaddr*)&socketAddress, &socketAdressLength);
	if (connectedSocketFD == -1)
	{
		std::cerr << "(Serveur)echec d'etablissement de la connexion" << std::endl;
		return ;
	}
    // add_to_poll_fds(poll_fds, client_fd, poll_count, poll_size);
	std::cout << "[Server] Accepted new connection on client socket " <<  connectedSocketFD << std::endl;
	sprintf(msg_to_send, "Welcome. You are client fd [%d]\n", connectedSocketFD);
    int status = send(connectedSocketFD, msg_to_send, strlen(msg_to_send), 0);
    if (status == -1) 
	{
		std::cout << "[Server] Send error to client " << connectedSocketFD << ": " << "message not send" << std::endl;
		return ;
    }
}

int main (void)
{
	//SOCK STREAM c'est pour indiquer qu'on va utiliser le protocole TCP
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD == -1)
	{
		std::cerr << "(Serveur)echec initialisation du socket" << std::endl;
		exit (1);
	}

	struct sockaddr_in socketAddress;
	socketAddress.sin_family = AF_INET;//indique qu'on veut une adresse IPV4
	socketAddress.sin_port = htons(LISTENING_PORT); //on indique le port par lequel le serveur ecoute
	socketAddress.sin_addr.s_addr = htonl(INADDR_ANY); // on accepte n'importe quelle adresse de connexion

	socklen_t socketAdressLength = sizeof(socketAddress);
	int bindReturnCode = bind(socketFD,(struct sockaddr*)&socketAddress, socketAdressLength);
	if (bindReturnCode == -1)
	{
		std::cerr << "(Serveur)echec liaison du socket" << std::endl;
		exit (1);
	}
	if (listen(socketFD, PENDING_QUEUE_MAXLENGTH) == -1)
	{
		std::cerr << "(Serveur)echec de demarage de l'ecoute des connexions entrantes" << std::endl;
		exit (1);
	}
	
	std::vector<pollfd>poll_fds;
	pollfd p;
	p.fd = socketFD;
	p.events = POLLIN;
	poll_fds.push_back(p);
	
    while (1) 
	{ 
        // Sonde les sockets prêtes (avec timeout de 2 secondes)
        int status = poll(poll_fds.data(), poll_fds.size(), 2000);
        if (status == -1) 
		{
           std::cerr << "(Serveur)echec de poll" << std::endl;
            exit(1);
        }
        else if (status == 0) 
		{
            // Aucun descipteur de fichier de socket n'est prêt
            std::cout << "En attente de nouvelles connexions..." << std::endl;
            continue;
        }

        // Boucle sur notre tableau de sockets
		
        for (size_t i = 0; i < poll_fds.size(); ++i) 
		{
            if (!(poll_fds[i].revents & POLLIN)) 
			{
				// La socket n'est pas prête à être lue
				// on s'arrête là et on continue la boucle
				continue ;
            }
			std::cout << "[" << socketFD << "]" << "Ready for I/O operation\n" << std::endl;
           
            // La socket est prête à être lue !
            if (poll_fds[i].fd == socketFD) {
                // La socket est notre socket serveur qui écoute le port
                accept_new_connection(socketFD, poll_fds, socketAddress, socketAdressLength);
            }
            // else {
            //     // La socket est une socket client, on va la lire
            //     read_data_from_socket(i, &poll_fds, poll_fds.size(), socketFD);
            // }
        }
    }	








	// int connectedSocketFD = accept(socketFD,(struct sockaddr*)&socketAddress, &socketAdressLength);
	// if (connectedSocketFD == -1)
	// {
	// 	std::cerr << "(Serveur)echec d'etablissement de la connexion" << std::endl;
	// 	exit (1);
	// }
	// //On va utiliser la memoire tampon pour receptionner le message
	// char buffer[BUFFER_SIZE] = {0};
	// int reveivedBytes = recv(connectedSocketFD, buffer, BUFFER_SIZE, 0);
	// if (reveivedBytes == -1)
	// {
	// 	std::cerr << "(Serveur)echec de reception du message du client" << std::endl;
	// 	exit (1);
	// }
	// std::cout << "Client: "<< buffer << std::endl;

	// const char message[] = "Bonjour, je suis le serveur!";
	// int sentBytes = send(connectedSocketFD, message, strlen(message), 0);
	// if (sentBytes == -1)
	// {
	// 	std::cerr << "(Serveur)echec de l'envoi du message au client" << std::endl;
	// 	exit (1);
	// }
	// close(connectedSocketFD);
	close(socketFD);
	return (0);

}
