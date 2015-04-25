
//
// Disclamer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resource, use the helper
// method resourcePath() from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

// Here is a small helper for you ! Have a look.
#include "ResourcePath.hpp"
#include <iostream>

struct Player {
    float x;
    float y;
};

struct Zombie {
    
    int id;
    float x;
    float y;
    
};

Player players[3];
Zombie zombies[3]; // max 3 zombies

void DrawDebug(sf::RenderWindow* win, int selfId)
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
        
        if(selfId == i)
        {
            s->setFillColor(sf::Color::Red);
        }
        else {
            s->setFillColor(sf::Color::Yellow);
        }
        win->draw(*s);
        delete s;
    }
}


int main(int, char const**)
{
    sf::RenderWindow w(sf::VideoMode(640,480), "Client");

    sf::TcpSocket socket;
    sf::IpAddress ip = "127.0.0.1";
    unsigned int port=30020;
    
    int myId=0;
    
    if(socket.connect(ip, port) == sf::Socket::Status::Done)
    {
        std::cout << "Polaczono" << std::endl;
        
        // get ID
        sf::Packet p;
        socket.receive(p);
        p >> myId;
        
        std::cout << "clients id: " << myId << std::endl;
    }
    else
    {
        std::cout << "Nie mozna!" << std::endl;
    }
    
    
    while(w.isOpen()) {
        
        for(int i = 0; i < 3; i++)
        {
            sf::Packet playerPacket;
            socket.receive(playerPacket);
            
            float a;
            float b;
            
            playerPacket >> a;
            playerPacket >> b;
            
            players[i].x = a;
            players[i].y = b;
        }
        
        // get zombies
        for(int i = 0; i < 3; i++)
        {
            sf::Packet zPacket;
            socket.receive(zPacket);
            
            float a;
            float b;
            
            zPacket >> a;
            zPacket >> b;
            
            zombies[i].x = a;
            zombies[i].y = b;
        }
        
        
        sf::Event e;
        while(w.pollEvent(e)) {
            if(e.type == sf::Event::Closed) {
                socket.disconnect();
                w.close();
            }
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            sf::Packet p;
            p << 1; // 1 move right
            socket.send(p);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            sf::Packet p;
            p << 2; // 1 move right
            socket.send(p);
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            sf::Packet p;
            p << 3; // 1 move right
            socket.send(p);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            sf::Packet p;
            p << 4; // 1 move right
            socket.send(p);
        }
        
        w.clear(sf::Color(100.0f,100.0f,100.0f));
        DrawDebug(&w,myId);
        w.display();
    }
    
    return EXIT_SUCCESS;
}
