#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

struct Player {
    float x;
    float y;
};

Player players[3]; // max 3 players

struct Zombie {
    
    int id;
    float x;
    float y;
    
};

struct Client {
    sf::TcpSocket* socket;
    int playerId;
    std::string name;
};

std::vector<Client> clients;

Zombie zombies[3]; // max 3 zombies

sf::SocketSelector socketSel;

void DrawDebug(sf::RenderWindow* win)
{
    for(int i = 0; i < 3; i++)
    {
        const Zombie& z = zombies[i];
        
        sf::Shape* s = new sf::CircleShape(16.0f);
        s->setPosition(z.x, z.y);
        s->setFillColor(sf::Color::Green);
        
        win->draw(*s);
        delete s;
    }
    
    for(int i = 0; i < 3; i++)
    {
        const Player& p = players[i];
        
        sf::Shape* s = new sf::CircleShape(16.0f);
        s->setPosition(p.x, p.y);
        s->setFillColor(sf::Color::Yellow);
        
        win->draw(*s);
        delete s;
    }
}

void SendPlayers(Client* client)
{
    for(int i = 0; i < 3; i++)
    {
        sf::Packet packet;
        packet << players[i].x << players[i].y;
        
        client->socket->send(packet);
        
    }
}

void SendZombies(Client* client)
{
    for(int i = 0; i < 3; i ++)
    {
        sf::Packet packet;
        packet << zombies[i].x << zombies[i].y;
        
        client->socket->send(packet);
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "ZGS");
    
    int playersConnectedToServer = 0;
    
    sf::TcpListener server;
     sf::Socket::Status s = server.listen(30020);
    socketSel.add(server);
  
   
    
    if(s == sf::Socket::Status::Done)
    {
        std::cout << "OK" << std::endl;
    }
    
    
    zombies[0].x = 50.0f;
    zombies[0].y = 50.0f;
    
    while(window.isOpen())
    {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            for(int i = 0; i < 3; i++)
            {
                players[i].x = 100.0f;
                players[i].y = 100.0f;
            }
        }
        
        zombies[0].x += 0.1f;
        
    

        
        if(socketSel.wait(sf::seconds(1.0f)))
       {
           for(int i = 0; i < clients.size(); i++)
           {
               SendPlayers( &clients[i] );
           }
           
           for(int i = 0; i < clients.size(); i++)
           {
               SendZombies( &clients[i] );
           }
            if(socketSel.isReady(server))
            {
                Client client;
                client.socket = new sf::TcpSocket;
                client.socket->setBlocking(false);
                client.playerId = playersConnectedToServer;
                server.accept(*client.socket);
                
                // wylsij id
                sf::Packet p;
                p << client.playerId;
                
                client.socket->send(p);
                
                clients.push_back(client);
                socketSel.add(*client.socket);
                
            
            
                
                std::cout << "New connection ID: " << playersConnectedToServer << std::endl;
                playersConnectedToServer++;
                
                // wyslij id
            }
            
            for(int i = 0; i < clients.size(); i++)
            {
                if( socketSel.isReady(*clients[i].socket) ) // has data?
                {
                    int messageId = 0;
                    
                    sf::Packet p;
                    clients[i].socket->receive(p);
                    p >> messageId;
                    
                    if(messageId == 1) // move right
                    {
                        players[ clients[i].playerId ].x += 1.0f;
                    }
                    if(messageId == 2) // move left
                    {
                        players[ clients[i].playerId ].x -= 1.0f;
                    }
                    if(messageId == 3) // move left
                    {
                        players[ clients[i].playerId ].y -= 1.0f;
                    }
                    if(messageId == 4) // move left
                    {
                        players[ clients[i].playerId ].y += 1.0f;
                    }
                    //std::cout << "MESSAGE: " << messageId << std::endl;
                    
                }
            }
        }
        
        sf::Event e;
        while(window.pollEvent(e)) {
            if(e.type == sf::Event::Closed) {
                
                server.close();
                window.close();
                
            }
        }
        
        window.clear( sf::Color(100,100,100) );
        
        DrawDebug(&window);
        
        window.display();
    }
    
    return 0;
}

